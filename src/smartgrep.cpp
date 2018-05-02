/**
 * @file	smartgrep.cpp
 * @author	pebble8888@gmail.com
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#else
#include <unistd.h>
#include <dirent.h>
#endif

#include "smartgrep.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <assert.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>

using namespace std;

const static int DATASIZE = 64 * 1024; // process unit size
const static int DATASIZE_OUT = DATASIZE*3/sizeof(wchar_t);
const static char kTab = 0x9;
const static char kSpace = 0x20;

static queue<string> m_queue;       // use m_queue_mtx for access
static bool  m_done_adding_files;   // use m_queue_mtx for access
static mutex m_queue_mtx;
static condition_variable m_files_ready_cond;
static mutex m_print_mtx;

void test( void )
{
	test_is_alnum_or_underscore();
}

static void search_worker(const int wordtype, const string word)
{
    while (true) {
        string s;
        {
            unique_lock<mutex> lk(m_queue_mtx);
            while (m_queue.empty()){
                if (m_done_adding_files) {
                    lk.unlock();
                    return; // exit thread
                }
                m_files_ready_cond.wait(lk);
            }
            s = m_queue.front();
            m_queue.pop();
        }
        parse_file(s.c_str(), wordtype, word.c_str());
    }
}

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	test();
#endif

	if( argc-1 < 2 ){
		usage();
		return 1;
	}

    bool use_repo = false;
    bool use_worker = true;
	int wordtype = 0;
    FILE_TYPE_INFO info;
    info.filetype = 0;
    info.typejs = true;
    info.typehtml = true;
	if( strcmp( argv[1], "-i" ) == 0 ){
		info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
		wordtype |= SG_WORDTYPE_NORMAL;
		wordtype |= SG_WORDTYPE_INCLUDE_COMMENT;
	} else if( strcmp( argv[1], "-h" ) == 0 ){
		info.filetype |= SG_FILETYPE_HEADER;
		wordtype |= SG_WORDTYPE_NORMAL;
		wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
	} else if( strcmp( argv[1], "-e" ) == 0 ){
		info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
		wordtype |= SG_WORDTYPE_NORMAL;
		wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
	} else if( strcmp( argv[1], "-iw" ) == 0 ){
		info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
		wordtype |= SG_WORDTYPE_WORD;
		wordtype |= SG_WORDTYPE_INCLUDE_COMMENT;
	} else if( strcmp( argv[1], "-hw" ) == 0 ){
		info.filetype |= SG_FILETYPE_HEADER;
		wordtype |= SG_WORDTYPE_WORD;
		wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
	} else if( strcmp( argv[1], "-ew" ) == 0 ){
		info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
		wordtype |= SG_WORDTYPE_WORD;
		wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
    } else if( strcmp( argv[1], "-c" ) == 0 ){
        info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
        wordtype |= SG_WORDTYPE_NORMAL;
        wordtype |= SG_WORDTYPE_INCLUDE_COMMENT;
        wordtype |= SG_WORDTYPE_CASEINSENSITIVE;
	} else {
		usage();
		return 1;
	}
    for( int i = 2; i < argc; ++i ){
        if( strcmp( argv[i], "-g" ) == 0 ){
            use_repo = true;
        } else if( strcmp( argv[i], "--nojs" ) == 0 ){
            info.typejs = false;
        } else if( strcmp( argv[i], "--nohtml" ) == 0 ){
            info.typehtml = false;
        } else if( strcmp( argv[i], "--ignore-dir" ) == 0 ){
            info.foldernamelist.add_foldername( argv[i+1] );
            ++i;
        } else if( strcmp( argv[i], "--noworker" ) == 0 ){
            use_worker = false;
        }
    }
	char path[512];
	memset( path, 0, sizeof(path) );
    if( use_repo ){
        // use repo folder
        smartgrep_getrepo( path, sizeof(path) );
    } else {
        // user current folder 
        smartgrep_getcwd( path, sizeof(path) );
    }
	char* word = argv[argc-1];
    
    // make worker
    int num_cores;
#ifdef _WIN32
    {           
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        num_cores = si.dwNumberOfProcessors;
    }   
#else
    num_cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
    int workers_len = num_cores-1;
    if( workers_len < 1 || !use_worker ){
        workers_len = 1;
    }
   
    m_done_adding_files = false;
    vector< shared_ptr<thread> > v_thread;
    for (int i = 0; i < workers_len; ++i) {
        shared_ptr<thread> th(new thread(search_worker, wordtype, string(word)));
        v_thread.push_back(th);
    }
    
#ifdef WIN32
	parse_directory_win( path, &info, wordtype, word );
#else
	parse_directory_mac( path, &info, wordtype, word );
#endif
    
    {
        lock_guard<mutex> lk(m_queue_mtx);
        m_done_adding_files = true;
        m_files_ready_cond.notify_all();
    }
    
    for (int i = 0; i < workers_len; ++i) {
        v_thread[i]->join();
    }

	return 0;
}

void smartgrep_getcwd( char* buf, size_t size )
{
#ifdef WIN32
	GetCurrentDirectory( size, buf );
#else
	getcwd( buf, size );
#endif
}

/**
 * @brief
 * @param char* buf
 * @param size_t size
 */
void smartgrep_getrepo( char* buf, size_t size )
{
    char curpath[512];
    memset( curpath, 0, sizeof(curpath) );
    smartgrep_getcwd( curpath, sizeof(curpath) );
    size_t idx = strlen(curpath);
    char path[512]; 
#ifdef WIN32
	assert( strlen(curpath) < size );
	strcpy( buf, curpath );
	while( idx > 3 ){
		for( int i = 0; i < 2; ++i ){
            // 0:.git
            // 1:.hg
			strcpy( path, curpath );
			path[idx] = '\0';
			if( i == 0 ) strcat( path, "\\.git" );
			else if( i == 1 ) strcat( path, "\\.hg" );
			// check
			HANDLE h_find;
			WIN32_FIND_DATA find_data;
			h_find = FindFirstFile( path, &find_data );
			if( h_find != INVALID_HANDLE_VALUE ){
				// can access
				FindClose( h_find );
				path[idx] = '\0';
				size_t len = strlen(path);
				assert( len < size );
				strcpy( buf, path );
				return;
			}
		}
		path[idx] = '\0';
		char* r = strrchr( path, '\\' );
		if( r == NULL ){
			strcpy( buf, curpath );
			return;
		}
		idx = (size_t)(r - path);
	}
#else
    while( true ){
        for( int i = 0; i < 2; ++i ){
            // 0:.git
            // 1:.hg
            strcpy( path, curpath );
            path[idx] = '\0'; 
            if( i == 0 ) strcat( path, "/.git" );
            else if( i == 1 ) strcat( path, "/.hg" );
            // check
            DIR* p_dir = opendir( path );
            if( p_dir != NULL ){
                // can access
                closedir( p_dir );
                path[idx] = '\0';
                size_t len = strlen(path);
                assert( len < size );
                strcpy( buf, path );
                return;
            }
        }
        path[idx] = '\0';
        char* r = strrchr( path, '/' );
        if( r == NULL ){
            strcpy( buf, curpath );
            return;
        }
        idx = (size_t)(r - path); 
    }
#endif
}

void usage( void )
{
	printf( 
		"Usage: smartgrep {-e[w]|-i[w]|-h[w]|-c} [options] word_you_grep\n"
        "\n"
		"  -e[w] : recursive [word] grep for supported file extensions excluding comment\n"
		"  -i[w] : recursive [word] grep for supported file extensions including comment\n"
		"  -h[w] : recursive [word] grep for .h excluding comment\n"
        "  -c : recursive case insensitive grep for supported file extesion including comment\n"
        "\n"
        " [options]\n"
        "  -g : use auto detect git or mercurial repository with the current directory\n"
        "  --nohtml : exclude .html file\n"
        "  --nojs : exclude .js file\n"
        "  --ignore-dir NAME : exclude NAME folder\n"
        "  --noworker : not use worker for sequencial output\n" 
        "\n"
		"  support file extensions : .cpp/.c/.mm/.m/.h/.js/.coffee/.rb/.py/.pl/.sh/.cr/.rs\n"
        "                            .java/.scala/.go/.cs/.xaml/.resx/.vb/.bas/.frm/.cls/.pc\n"
        "                            .php/.plist/.pbxproj/.strings/.storyboard/.swift/.gyb/.vim/\n"
        "                            .css/.scss\n"
        "  asis support file extensions : .erb/.html/.xml\n"
        "\n"
        "  ignore directory : .git/.hg/.svn/.vs\n"
        "\n"
        "  Version 4.2.3\n"
	);
}

#ifdef WIN32
/*
 * @param char* path
 * @param int   filetype	SG_FILETYPE_SOURCE: .c/.cpp/.m/.mm/etc
 * 							SG_FILETYPE_HEADER: .h/.hpp/etc
 * @param int   wordtype	
 * @param char* target_word
 */
void parse_directory_win( char* path, FILE_TYPE_INFO* p_info, int wordtype, const char* target_word )
{
	char path_name[512];
	strcpy( path_name, path );
	strcat( path_name, "\\*.*" );
	
	HANDLE h_find;
	WIN32_FIND_DATA find_data;
	h_find = FindFirstFile( path_name, &find_data );
	if( h_find == INVALID_HANDLE_VALUE ){
		printf( "directory read error! [%s]\n", path );
		return;
	}
	while( true ){
		if( strcmp( find_data.cFileName, "." ) == 0 ||
			strcmp( find_data.cFileName, ".." ) == 0 ){
			// do nothing
			;
		}
		else if( (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) &&
		         find_data.cFileName[0] != '.' ){
			// not hidden directory
            if( p_info->foldernamelist.has_foldername( find_data.cFileName ) ){
                // ignore the folder
            } else {
                char path_name_r[512];
                strcpy( path_name_r, path );
                strcat( path_name_r, "\\" );
                strcat( path_name_r, find_data.cFileName );
                parse_directory_win( path_name_r, p_info, wordtype, target_word );
            }
		} else if( ( (p_info->filetype & SG_FILETYPE_SOURCE ) && is_source_file( p_info, find_data.cFileName ) ) ||
				   ( (p_info->filetype & SG_FILETYPE_HEADER ) && is_header_file( find_data.cFileName ) ) ){
            // file
			char file_name_r[512];
			strcpy( file_name_r, path );
			strcat( file_name_r, "\\" );
			strcat( file_name_r, find_data.cFileName );
            {
                lock_guard<mutex> lk(m_queue_mtx);
                m_queue.push(string(file_name_r));
                m_files_ready_cond.notify_one();
            }
		}
		BOOL ret = FindNextFile( h_find, &find_data );
		if( !ret ){
			DWORD dwError = GetLastError();
			assert( dwError == ERROR_NO_MORE_FILES );
			break;
		}
	}
	FindClose( h_find );
}

#else

/*
 * @param char* path
 * @param int   filetype	SG_FILETYPE_SOURCE: .c/.cpp/.m/.mm/.cs/.js etc
 * 							SG_FILETYPE_HEADER: .h/.hpp/etc
 * @param int   wordtype
 * @param char* target_word
 */
void parse_directory_mac( char* path, FILE_TYPE_INFO* p_info, int wordtype, const char* target_word )
{
	DIR* p_dir = opendir( path );
	if( p_dir == NULL ){
		printf( "directory read error! [%s]\n", path );
		return;
	}
	struct dirent* p_dirent;
    struct stat file_info;
	while( (p_dirent = readdir( p_dir )) != NULL ){
        char path_name_r[512];
        strcpy( path_name_r, path );
        strcat( path_name_r, "/" );
        strcat( path_name_r, p_dirent->d_name );
        lstat(path_name_r, &file_info);

		if( strcmp( p_dirent->d_name, "." ) == 0 ||
		   strcmp( p_dirent->d_name, ".." ) == 0 ){
			// do nothing
			;
		}
		else if(S_ISDIR(file_info.st_mode) &&
				p_dirent->d_name[0] != '.' ){
			// not hidden directory
            if( p_info->foldernamelist.has_foldername( p_dirent->d_name ) ){
                // ignore the folder
            } else {
                parse_directory_mac( path_name_r, p_info, wordtype, target_word );
            }
		} else if( ( (p_info->filetype & SG_FILETYPE_SOURCE ) && is_source_file( p_info, p_dirent->d_name ) ) ||
				  ( (p_info->filetype & SG_FILETYPE_HEADER ) && is_header_file( p_dirent->d_name ) ) ){
            {
                lock_guard<mutex> lk(m_queue_mtx);
                m_queue.push(string(path_name_r));
                m_files_ready_cond.notify_one();
            }
        }
	}
	closedir( p_dir );	
}
#endif

bool is_cs_file( const char* file_name ){
    return is_ext( file_name, "cs" );
}

bool is_source_file( FILE_TYPE_INFO* p_info, const char* file_name ){
	if( is_ext( file_name, "c" ) ||
		is_ext( file_name, "cpp" ) || 
        is_ext( file_name, "cc" ) ||
		is_ext( file_name, "cxx" ) ||
		is_ext( file_name, "tli" ) ||
		is_ext( file_name, "inc" ) ||
		is_ext( file_name, "rc" ) ||
		is_ext( file_name, "m" ) ||
		is_ext( file_name, "mm" ) ||
	    (is_ext( file_name, "cs" ) && !is_last(file_name, ".g.cs") && !is_last(file_name, ".g.i.cs")) ||
        is_ext( file_name, "xaml" ) ||
        is_ext( file_name, "resx" ) ||
		(p_info->typejs && is_ext( file_name, "js")) ||
		is_ext( file_name, "java" ) ||
        is_ext( file_name, "scala" ) ||
		is_ext( file_name, "go" ) ||
        is_ext( file_name, "rs" ) ||
        is_ext( file_name, "swift" ) ||
        is_ext( file_name, "gyb" ) ||
        is_ext( file_name, "css" ) ||
        is_ext( file_name, "scss" ) ||
		is_ext( file_name, "pc" ) ||
        is_ext( file_name, "php" ) ||
        is_shell_file( file_name ) ||
        is_ruby_file( file_name ) ||
        is_crystal_file( file_name ) ||
        is_erb_file( file_name ) ||
        (p_info->typehtml && is_html_file( file_name )) ||
        is_xml_file( file_name) ||
        is_coffee_file( file_name ) ||
	    is_python_file( file_name ) ||
        is_perl_file( file_name ) ||
        is_vb_file( file_name ) ||
        is_vim_file( file_name ) ||
        is_xcode_resource_file( file_name ) ){
		return true;
	}
	return false;
}

bool is_shell_file( const char* file_name ){ return is_ext( file_name, "sh" ); }
bool is_ruby_file( const char* file_name ){ return is_ext( file_name, "rb" ); }
bool is_crystal_file( const char* file_name ){ return is_ext( file_name, "cr" ); }
bool is_erb_file( const char* file_name ){ return is_ext( file_name, "erb" ); }
bool is_html_file( const char* file_name ){ return is_ext( file_name, "html" ); }
bool is_xml_file( const char* file_name ){ return is_ext( file_name, "xml" ); }
bool is_coffee_file( const char* file_name ){ return is_ext( file_name, "coffee" ); }
bool is_python_file( const char* file_name ){ return is_ext( file_name, "py" ); }
bool is_perl_file( const char* file_name ){ return is_ext( file_name, "pl" ); }
bool is_vim_file( const char* file_name ){ return is_ext( file_name, "vim" ); }
bool is_vb_file( const char* file_name ){
    if( is_ext( file_name, "vb" ) ||
        is_ext( file_name, "frm" ) ||
        is_ext( file_name, "bas" ) ||
        is_ext( file_name, "cls" ) ){
        return true;
    } else {
        return false;
    }
}
bool is_xcode_resource_file( const char* file_name ){
    if( is_ext( file_name, "pbxproj" ) ||
        is_ext( file_name, "strings" ) ||
        is_ext( file_name, "plist" ) ||
        is_ext( file_name, "storyboard" ) ){
        return true;
    }
    return false;
}

bool is_header_file( const char* file_name ){
	if( is_ext( file_name, "h" ) ||
        is_ext( file_name, "hh" ) ||
		is_ext( file_name, "hpp" ) ||
		is_ext( file_name, "hxx" ) ||
		is_ext( file_name, "tlh" ) ||
		is_ext( file_name, "inl" ) ||
        is_ext( file_name, "pro" ) ){
		return true;
	} else {
		return false;
	}
}

/*
 * @param char: ext_name "c", "cpp", "h", etc
 */
bool is_ext( const char* file_name, const char* ext_name ){
	char* period = strrchr( (char*)file_name, '.' );
	if( period == NULL ){
		return false;
	}
	char buf[512];
	memset( buf, 0, sizeof(buf) );
	char* p;
	char* q;
	for( p = period + 1, q = buf; *p != '\0'; ++p, ++q ){
		*q = tolower( *p );
	}
	return ( strcmp( buf, ext_name ) == 0 );
}

bool is_last(const char* file_name, const char* last_name) {
    int i = strlen(file_name);
    int len = strlen(last_name);
    if (len <= 0) {
        return false;
    }
    i -= 1;
    int j = len - 1;
    while (i >= 0 && j >= 0) {
        if (file_name[i] == last_name[j]) {
            if (j == 0) {
                return true;
            }
        } else {
            return false;
        }
        i -= 1;
        j -= 1;
    }
    return false;
}

/*
 * @brief	parse one file
 * 			output to standard out
 * @param [in] char* file_name
 * @param [in] int wordtype
 * @param [in] char* target_word
 */
void parse_file( const char* file_name, int wordtype, const char* target_word )
{
	// file extension
	int file_extension;
    if( is_shell_file( file_name ) ||
        is_crystal_file( file_name ) ){
        file_extension = kShell;
    } else if( is_ruby_file( file_name ) ){
		file_extension = kRuby; 
    } else if( is_coffee_file( file_name ) ){
        file_extension = kCoffee;
	} else if( is_python_file( file_name ) ){
		file_extension = kPython;
    } else if( is_perl_file( file_name ) ){
        file_extension = kPerl;
    } else if( is_vb_file( file_name ) ){
        file_extension = kVB;
    } else if( is_erb_file( file_name ) ||
               is_html_file( file_name ) ||
               is_xml_file( file_name ) ){
        file_extension = kAsIs;
    } else if( is_vim_file( file_name ) ){
        file_extension = kVim;
	} else {
		file_extension = kC; 
	}

	FILE* fp = fopen( file_name, "rb" );
	if( fp == NULL )
		return;

#ifndef NOFEAT_UTF16
    bool is_utf16 = false;
    // check BOM for UTF16
    if( is_cs_file( file_name ) ||
        is_xcode_resource_file( file_name ) ){
        while( true ){
            int data[2];
            data[0] = fgetc( fp );
            if( feof( fp ) || ferror( fp ) ) break;
            data[1] = fgetc( fp );
            if( feof( fp ) || ferror( fp ) ) break;

            if( data[0] == 0xff && data[1] == 0xfe ){
                is_utf16 = true;
            }
            break;
        }
        // back to begin
        fseek( fp, 0, SEEK_SET );
    }
#endif

	bool isin_multiline_comment = false;
	PREP prep;

	// it is presumed that the one line byte size in file don't exceed 64k
    char* p_data = new char[DATASIZE+1];
#ifndef NOFEAT_UTF16
    char* q_data = NULL;
    if( is_cs_file( file_name ) ||
        is_xcode_resource_file( file_name ) ){
        q_data = new char[DATASIZE_OUT+1];
    }
    int q_datasize = 0;
#endif

	int lineno;
    char* r_data = NULL;
    size_t r_datasize = 0;
    
    const int BOMSIZE = 2;

	for( lineno = 1; ;){
        if( r_datasize <= 0 ){
            // no buffer unprocessed, so read new data.
            memset( p_data, 0, DATASIZE );
            size_t sz = fread( p_data, 1, DATASIZE, fp );
            if( sz <= 0 ) break;
#ifndef NOFEAT_UTF16
            if( is_utf16 ){
                
                // number of wchar_t elements
                size_t p_datasize = (sz-BOMSIZE)/sizeof(int16_t);

                if( p_datasize == 0 ) break;

                memset( q_data, 0, DATASIZE_OUT );
                q_datasize = UTF16LEToUTF8( (int16_t*)(p_data+BOMSIZE), (int)p_datasize, q_data );
                
                r_data = q_data;
                r_datasize = q_datasize;
            } else
#endif
            {
                r_data = p_data;
                r_datasize = sz;
            }
        }

		bool found;
		if( wordtype & SG_WORDTYPE_EXCLUDE_COMMENT ){
			if( file_extension == kC ){
				found = process_line_exclude_comment_c( &isin_multiline_comment, &prep,
														r_data, r_datasize, wordtype, target_word );
			} else if( file_extension == kShell ||
                       file_extension == kRuby ||
                       file_extension == kCoffee ||
                       file_extension == kPython ||
                       file_extension == kPerl ){
				found = process_line_exclude_comment_ruby( &isin_multiline_comment,
                                                        r_data, r_datasize, wordtype, target_word,
                                                        file_extension );
            } else if( file_extension == kVB ){
                found = process_line_exclude_comment_vb( r_data, r_datasize, wordtype, target_word );
            } else if( file_extension == kVim ){
                found = process_line_exclude_comment_vim( r_data, r_datasize, wordtype, target_word );
            } else if( file_extension == kAsIs ){
                found = process_line_include_comment( r_data, r_datasize, wordtype, target_word );
			} else {
				assert( false );
			}
		} else if( wordtype & SG_WORDTYPE_INCLUDE_COMMENT ){
			found = process_line_include_comment( r_data, r_datasize, wordtype, target_word );
		} else {
			assert( false );
		}
        bool found_linebreak = false;
		if( found ){
            // found target
            char* q = r_data;
            {
                lock_guard<mutex> lk(m_print_mtx);
                // output filename and line number or EOF
    			printf( "%s:%d:", file_name, lineno );
                // output until next newline or EOF
                for( ; q < r_data + r_datasize; ++q ){
                    if( *q == '\n' ){
                        // found unix newline
                        ++q;
                        break;
                    } else if( *q == '\r' &&
                               q+1 < r_data + r_datasize &&
                              *(q+1) == '\n' ){
                        // found windows newline
                        q += 2;
                        break;
                    }
    				printf( "%c", *q );
    			}
                if( q < r_data + r_datasize ){
                    found_linebreak = true;
                } else {
                    // almost EOF
                    // TODO:not a completely valid logic
                }
                printf( "\n" );
            }
            // proceed processed buffer size
            const size_t advance = (size_t)(q-r_data);
            r_data += advance;
            r_datasize -= advance;
		} else {
            // not found target
            // proceed to newline
            char* q = r_data;
            for( ; q < r_data + r_datasize; ++q ){
                if( *q == '\n' ){
                    // found unit newline
                    ++q;
                    break;
                } else if( *q == '\r' &&
                           q+1 < r_data + r_datasize &&
                          *(q+1) == '\n' ){
                    // found windows newline
                    q += 2;
                    break;
                }
            }
            if( q < r_data + r_datasize ){
                found_linebreak = true;
            }
            // proceed processed buffer size
            const size_t advance = (size_t)(q-r_data);
            r_data += advance;
            r_datasize -= advance;
        }
        if( found_linebreak ){
            ++lineno;
        }
	}
    delete [] p_data;
#ifndef NOFEAT_UTF16
    delete [] q_data;
#endif
	fclose( fp );
}

/*
 * @brief	parse one line in file excluding comemnt
 *
 * @retval 	true : found
 * @retval	false: not found
 *
 * @param	[in/out] 	bool* 	p_isin_multiline_comment 		whether in C comment
 * @param   [in/out]    PREP* 	p_prep
 * @param	[in]		char* 	buf
 * @param	[in]		size_t	bufsize
 * @param	[in]		int 	wordtype
 * @param	[in]		char* 	target_word
 */
bool process_line_exclude_comment_c( bool* p_isin_multiline_comment, PREP* p_prep,
									char* buf, size_t bufsize, int wordtype, const char* target_word )
{
	char valid_str[DATASIZE_OUT+1];


	bool isin_literal = false; // "xxx", 'xxx'
    char* p = buf;
	char* q = valid_str;
    while (p < buf + bufsize && q < &valid_str[DATASIZE_OUT+1]){
		if( *p == '\n' || *p == '\0' ) break;
		if( !isin_literal && !(*p_isin_multiline_comment) && !(p_prep->is_commented())
			&& *p == '/' && *(p+1) == '/'){
			// C++ comment
			break;
		} else if(!isin_literal && !(*p_isin_multiline_comment) && !(p_prep->is_commented())
					&& *p == '/' && *(p+1) == '*'){
			// the begining of C comment
			p += 2;
			*p_isin_multiline_comment = true;
			continue;
		} else if( !isin_literal && *p_isin_multiline_comment && !(p_prep->is_commented())){
			// in c comment
			while( p < buf + bufsize ){
				if( *p == '\n' || *p == '\0' ) goto WHILEOUT;
				if( *p == '*' && *(p+1) == '/'){
					break;
				}
				++p;
			}
			p += 2;
			// the end of c comment
			*p_isin_multiline_comment = false;
		} else if( !(*p_isin_multiline_comment) && !(p_prep->is_commented())
					&& ( *p == '\"' || *p == '\'') && (p == buf || *(p-1) != '\\' ) ) {
			// reverse isin_literal
			isin_literal = !isin_literal;
		} else if( !isin_literal && !(*p_isin_multiline_comment) && *p == '#' ){
			if( memcmp( p, SG_PREP_IF, strlen(SG_PREP_IF) ) == 0 ||
				memcmp( p, SG_PREP_IFDEF, strlen(SG_PREP_IFDEF) ) == 0 ||
				memcmp( p, SG_PREP_IFNDEF, strlen(SG_PREP_IFNDEF) ) == 0 ){
				// #if or #ifdef or #ifndef
				if( memcmp( p, SG_PREP_IFZERO, strlen(SG_PREP_IFZERO)) == 0 ) {
					// #if 0
					p_prep->push( true );
					p += strlen(SG_PREP_IFZERO);
				} else {
					p_prep->push( false );
					if( memcmp( p, SG_PREP_IF, strlen(SG_PREP_IF) ) == 0 ){
						p += strlen(SG_PREP_IF);
					} else if( memcmp( p, SG_PREP_IFDEF, strlen(SG_PREP_IFDEF) ) == 0 ){
						p += strlen(SG_PREP_IFDEF);
					} else if( memcmp( p, SG_PREP_IFNDEF, strlen(SG_PREP_IFNDEF) ) == 0 ){
						p += strlen(SG_PREP_IFNDEF);
					} else {
						assert( false );
					}
				}
				continue;
			} else if( p_prep->can_change_to_else()
					   && ( memcmp( p, SG_PREP_ELIF, strlen(SG_PREP_ELIF) ) == 0 ||
						    memcmp( p, SG_PREP_ELSE, strlen(SG_PREP_ELSE) ) == 0 ) ){
				p_prep->change_to_else();

				if( memcmp( p, SG_PREP_ELIF, strlen(SG_PREP_ELIF) ) == 0 ){
					p += strlen(SG_PREP_ELIF);
				} else if( memcmp( p, SG_PREP_ELSE, strlen(SG_PREP_ELSE) ) == 0 ){
					p += strlen(SG_PREP_ELSE);
				} else {
					assert( false );
				}
				continue;
			} else if( memcmp( p, SG_PREP_ENDIF, strlen(SG_PREP_ENDIF) ) == 0 ){
				// #endif
				p_prep->pop();
				p += strlen(SG_PREP_ENDIF);
				continue;
			}
		} else if( !isin_literal && !(*p_isin_multiline_comment) && p_prep->is_commented() ){
            ++p;
			continue;
		}
		if( *p == '\r' || *p == '\n' || *p == '\0' ) break;
		
		// valid data
		*(q++) = *(p++);
	}
WHILEOUT:
    *q = 0x0; // null terminate
	return findword_in_line( valid_str, wordtype, target_word );
}

/**
 * dynamic languages
 */
bool process_line_exclude_comment_ruby( bool* p_isin_multiline_comment,
                                char* buf, size_t bufsize, int wordtype, const char* target_word,
                                int file_extension )
{
	char valid_str[DATASIZE_OUT];
	bool isin_dq = false; // "xxx"
	bool isin_sq = false; // 'xxx'
	bool isin_var = false; // "#{}"
	char* q = valid_str;
    for (char* p = buf; p < buf + bufsize; ++p){
		if( *p == '\n' || *p == '\0' ) break; 

        if( !(*p_isin_multiline_comment)
            && !isin_dq && !isin_sq
            && ((file_extension == kRuby && p == buf && memcmp(p, "=begin", 5) == 0) ||
                (file_extension == kPerl && p == buf && memcmp(p, "=pod", 4) == 0 ) ||
                (file_extension == kCoffee && memcmp(p, "###", 3) == 0) ||
                (file_extension == kPython &&
                  ( memcmp(p, "\"\"\"", 3) == 0 || memcmp(p, "'''", 3) == 0 ) ) ) )
        {
            // the begining of multi-line comment
            if( file_extension == kRuby ){ p += 5; }
            else if( file_extension == kPerl ){ p += 4; }
            else if( file_extension == kCoffee ){ p += 3; }
            else if( file_extension == kPython ){ p += 3; }
            *p_isin_multiline_comment = true;
            continue;
        } else if( !isin_dq && !isin_sq && *p_isin_multiline_comment ){
            // in multi-line comment
            bool found = false;
            while( p < buf + bufsize ){
                if( *p == '\n' ) goto WHILEOUT;
                if( file_extension == kRuby && p == buf && memcmp(p, "=end", 4) == 0 ){
                    p += 4;
                    found = true;
                    break;
                } else if( file_extension == kPerl && p == buf && memcmp(p, "=cut", 4) == 0 ){
                    p += 4;
                    found = true;
                    break;
                } else if( file_extension == kCoffee && memcmp(p, "###", 3) == 0 ){
                    p += 3;
                    found = true;
                    break;
                } else if( file_extension == kPython && 
                           ( memcmp(p, "\"\"\"",3) == 0 || memcmp(p, "'''",3) == 0 ) ){
                    p += 3;
                    found = true;
                    break;
                }
                ++p;
            }
            if (found){
                // the end of multi-line comment
                *p_isin_multiline_comment = false;
            }
        } else if( !(*p_isin_multiline_comment)
            && !isin_sq && !isin_dq && *p == '#' ){
			// single-line comment
			break;
		} else if( !(*p_isin_multiline_comment)
                   && !isin_sq && *p == '\"' && ( p == buf || *(p-1) != '\\' ) ){
			// reverse isin_dq
			isin_dq = !isin_dq;
		} else if( !(*p_isin_multiline_comment)
                   && !isin_dq && *p == '\'' && ( p == buf || *(p-1) != '\\' ) ){
			// reverse isin_sq
			isin_sq = !isin_sq;
		} else if( !(*p_isin_multiline_comment)
                   && isin_dq && !isin_var && *p == '#' && *(p+1) == '{' ){
			isin_var = true;
		} else if( !(*p_isin_multiline_comment)
                   && isin_dq && file_extension == kRuby && isin_var && *p == '}' ){
			isin_var = false;
		}
		if( *p == '\r' || *p == '\n' || *p == '\0' ) break;
		
		// valid data
		*(q++) = *p;
	}

WHILEOUT:
    *q = 0x0; // null terminate
	return findword_in_line( valid_str, wordtype, target_word );
}

/**
 * visual basic 6 and visual basic dot net
 */
bool process_line_exclude_comment_vb( char* buf, size_t bufsize, int wordtype, const char* target_word )
{
	char valid_str[DATASIZE_OUT+1];
	bool isin_dq = false; // "xxx"
	char* q = valid_str;
    for (char* p = buf; p < buf + bufsize; ++p){
		if( *p == '\n' || *p == '\0' ) break; 

        if( !isin_dq && *p == '\'' ){
			// single-line comment
			break;
		} else if( *p == '\"' && ( p == buf || *(p-1) != '\\' ) ){
			// reverse isin_dq
			isin_dq = !isin_dq;
		}
		if( *p == '\r' || *p == '\n' || *p == '\0' ) break;
		
		// valid data
		*(q++) = *p;
	}
WHILEOUT:
    *q = 0x0; // null terminate
	return findword_in_line( valid_str, wordtype, target_word );
}

/**
 * vim script
 */
bool process_line_exclude_comment_vim( char* buf, size_t bufsize, int wordtype, const char* target_word )
{
	char valid_str[DATASIZE_OUT+1];
	bool found_anything_but_whitespace = false;
    char* q = valid_str;
    for (char* p = buf; p < buf + bufsize; ++p){
		if( *p == '\n' || *p == '\0' ) break; 

        if( !found_anything_but_whitespace && *p == '\"' ){
			// single-line comment
			break;
		}

		if( *p == '\r' ) break;

		if( !(*p == kSpace || *p == kTab) ) found_anything_but_whitespace = true;
		
		// valid data
		*(q++) = *p;
	}
WHILEOUT:
    *q = 0x0; // null terminate
    return findword_in_line( valid_str, wordtype, target_word );
}

/*
 * @brief	search string in one line.
 * 			strstr can be hit twice in one line.
 *
 * @retval	true : found target
 * @retval	false: not found target
 *
 * @param	[in] char* 	valid_str       

 * @param	[in] int 	wordtype
 * @param	[in] char* 	target_word
 */
bool findword_in_line( char* valid_str, int wordtype, const char* target_word )
{
	if( wordtype & SG_WORDTYPE_NORMAL ){
		// normal search
        if( wordtype & SG_WORDTYPE_CASEINSENSITIVE ){
#ifdef WIN32
            return strstr( valid_str, target_word ) != NULL;
#else
            return strcasestr( valid_str, target_word ) != NULL;
#endif
        } else {
            return strstr( valid_str, target_word ) != NULL;
        }
	} else if( wordtype & SG_WORDTYPE_WORD ){
		// word search
		int   target_word_len = (int)strlen(target_word);
		char* remain_ptr = valid_str;
		char* findptr = strstr( valid_str, target_word );

		while( findptr != NULL ){
			bool  head = false;
			bool  tail = false;
			if( findptr == remain_ptr ){
				head = true;
			} else {
				head = !is_alnum_or_underscore( *(findptr-1) );
			}

			if( head ){
				int   remain_str_len = (int)strlen(remain_ptr);
				if( remain_ptr + remain_str_len == findptr + target_word_len ){
					tail = true;
				} else {
					tail = !is_alnum_or_underscore( *(findptr + target_word_len) );
				}
				if( tail ){
					return true;
				}
			}
			remain_ptr = findptr;
			findptr = strstr( findptr + target_word_len , target_word );
		}
		return false;
	}
	assert( false );
	return false;
}

/**
 * @brief   as is
 * @retval	true: found target
 * @retval	false:not found target
 * @param [in] char* buf
 * @param [in] int worktype
 * @param [in] char* target_word
 */
bool process_line_include_comment( char* buf, size_t bufsize, int wordtype, const char* target_word )
{
    char valid_str[DATASIZE_OUT+1];
    char* q = valid_str;
    for (char* p = buf ; p < buf + bufsize && q < &valid_str[DATASIZE_OUT+1]; ++p){
        if( *p == '\0' || *p == '\r' || *p  =='\n' ){
            break;
        }
        *(q++) = *p;
    }
    *q = 0x0; // null terminate
	return findword_in_line(valid_str, wordtype, target_word);
}

void test_is_alnum_or_underscore( void )
{
	assert(  is_alnum_or_underscore( 'a' ) );
	assert(  is_alnum_or_underscore( 'A' ) );
	assert(  is_alnum_or_underscore( '1' ) );
	assert(  is_alnum_or_underscore( '_' ) );
	assert( !is_alnum_or_underscore( '%' ) );
}

bool is_alnum_or_underscore( int val )
{
	return isalnum( val ) ? true : val == '_';	
}

/**
 * @return bytelength
 */
int UTF16LEToUTF8( int16_t* pwIn, int count, char* pOut )
{
    int16_t* pw = pwIn;
    char* q = pOut;
    while( pw <  pwIn + count ){
        if( *pw <= 0x7f ){
            *(q++) = (char)*pw;
            if( *pw == 0x0 ){
                break;
            }
        } else if( *pw <= 0x7ff ){
            *(q++) = (char)((*pw >> 6) | 0xc0);
            *(q++) = (char)((*pw & 0x3f) | 0x80);
        } else {
            *(q++) = (char)((*pw >> 12) | 0xe0);
            *(q++) = (char)((*pw >> 6 & 0x3f) | 0x80);
            *(q++) = (char)((*pw & 0x3f) | 0x80);
        }
        ++pw;
    }
    return (int)(q-pOut);
}

