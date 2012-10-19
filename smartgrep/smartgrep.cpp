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

const static int DATASIZE = 64 * 1024;

void test( void )
{
	test_is_alnum_or_underscore();
}

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	test();
#endif

	if( argc - 1 != 2 ){
		usage();
		return 1;
	}

	int wordtype = 0;
	int filetype = 0;
	if( strcmp( argv[1], "/n" ) == 0 ){
		filetype |= SMARTGREP_FILETYPE_SOURCE;
		filetype |= SMARTGREP_FILETYPE_HEADER;
		wordtype |= SMARTGREP_WORDTYPE_NORMAL;
		wordtype |= SMARTGREP_WORDTYPE_INCLUDE_COMMENT;
	} else if( strcmp( argv[1], "/h" ) == 0 ){
		filetype |= SMARTGREP_FILETYPE_HEADER;
		wordtype |= SMARTGREP_WORDTYPE_NORMAL;
		wordtype |= SMARTGREP_WORDTYPE_EXCLUDE_COMMENT;
	} else if( strcmp( argv[1], "/b" ) == 0 ){
		filetype |= SMARTGREP_FILETYPE_SOURCE;
		filetype |= SMARTGREP_FILETYPE_HEADER;
		wordtype |= SMARTGREP_WORDTYPE_NORMAL;
		wordtype |= SMARTGREP_WORDTYPE_EXCLUDE_COMMENT;
	} else if( strcmp( argv[1], "/nw" ) == 0 ){
		filetype |= SMARTGREP_FILETYPE_SOURCE;
		filetype |= SMARTGREP_FILETYPE_HEADER;
		wordtype |= SMARTGREP_WORDTYPE_WORD;
		wordtype |= SMARTGREP_WORDTYPE_INCLUDE_COMMENT;
	} else if( strcmp( argv[1], "/hw" ) == 0 ){
		filetype |= SMARTGREP_FILETYPE_HEADER;
		wordtype |= SMARTGREP_WORDTYPE_WORD;
		wordtype |= SMARTGREP_WORDTYPE_EXCLUDE_COMMENT;
	} else if( strcmp( argv[1], "/bw" ) == 0 ){
		filetype |= SMARTGREP_FILETYPE_SOURCE;
		filetype |= SMARTGREP_FILETYPE_HEADER;
		wordtype |= SMARTGREP_WORDTYPE_WORD;
		wordtype |= SMARTGREP_WORDTYPE_EXCLUDE_COMMENT;
	} else {
		usage();
		return 1;
	}
	char path[512];
	memset( path, 0, sizeof(path) );
	smartgrep_getcwd( path, sizeof(path) );
	char* word = argv[2];
#ifdef WIN32
	parse_directory_win( path, filetype, wordtype, word );
#else
	parse_directory_mac( path, filetype, wordtype, word );
#endif
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


void usage( void )
{
	printf( 
		"Usage: smartgrep /h {word}  : recursive      grep for .h                        excluding comment\n"
		"                 /b {word}  : recursive      grep for .cpp .c .mm .m .h .cs .js excluding comment\n"
		"                 /n {word}  : recursive      grep for .cpp .c .mm .m .h .cs .js including comment\n"
		"                 /hw {word} : recursive word grep for .h                        excluding comment\n"
		"                 /bw {word} : recursive word grep for .cpp .c .mm .m .h .cs .js excluding comment\n"
		"                 /nw {word} : recursive word grep for .cpp .c .mm .m .h .cs .js including comment\n"
	);
	print_version();
}

#ifdef WIN32
/*
 * @param char* path
 * @param int   filetype	SMARTGREP_FILETYPE_SOURCE: .c/.cpp/.m/.mm/etc
 * 							SMARTGREP_FILETYPE_HEADER: .h/.hpp/etc
 * @param int   wordtype	SMARTGREP_WORDTYPE_WORD_EXCLUDE_COMMENT: \<{word}\>
 * 							SMARTGREP_WORDTYPE_NORMAL_EXCLUDE_COMMENT: word
 * 							SMARTGREP_WORDTYPE_NORMAL_INCLUDE_COMMENT: word
 * @param char* target_word
 */
void parse_directory_win( char* path, int filetype, int wordtype, char* target_word )
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
			// not hided directory
			char path_name_r[512];
			strcpy( path_name_r, path );
			strcat( path_name_r, "\\" );
			strcat( path_name_r, find_data.cFileName );
			parse_directory_win( path_name_r, filetype, wordtype, target_word );
		} else if( ( (filetype & SMARTGREP_FILETYPE_SOURCE ) && is_source_file( find_data.cFileName ) ) ||
				   ( (filetype & SMARTGREP_FILETYPE_HEADER ) && is_header_file( find_data.cFileName ) ) ){
			char file_name_r[512];
			strcpy( file_name_r, path );
			strcat( file_name_r, "\\" );
			strcat( file_name_r, find_data.cFileName );
			parse_file( file_name_r, wordtype, target_word ); 
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
 * @param int   filetype	SMARTGREP_FILETYPE_SOURCE: .c/.cpp/.m/.mm/.cs/.js etc
 * 							SMARTGREP_FILETYPE_HEADER: .h/.hpp/etc
 * @param int   wordtype	SMARTGREP_WORDTYPE_WORD_EXCLUDE_COMMENT: \<{word}\>
 * 							SMARTGREP_WORDTYPE_NORMAL_EXCLUDE_COMMENT: word
 * 							SMARTGREP_WORDTYPE_NORMAL_INCLUDE_COMMENT: word
 * @param char* target_word
 */
void parse_directory_mac( char* path, int filetype, int wordtype, char* target_word )
{
	char path_name[512];
	strcpy( path_name, path );
	strcat( path_name, "\\*.*" );
	
	DIR* p_dir = opendir( path );
	if( p_dir == NULL ){
		printf( "directory read error! [%s]\n", path );
		return;
	}
	struct dirent* p_dirent;
	while( (p_dirent = readdir( p_dir )) != NULL ){
		if( strcmp( p_dirent->d_name, "." ) == 0 ||
		   strcmp( p_dirent->d_name, ".." ) == 0 ){
			// do nothing
			;
		}
		else if( (p_dirent->d_type == DT_DIR ) &&
				p_dirent->d_name[0] != '.' ){
			// not hided directory
			char path_name_r[512];
			strcpy( path_name_r, path );
			strcat( path_name_r, "/" );
			strcat( path_name_r, p_dirent->d_name );
			parse_directory_mac( path_name_r, filetype, wordtype, target_word );
		} else if( ( (filetype & SMARTGREP_FILETYPE_SOURCE ) && is_source_file( p_dirent->d_name ) ) ||
				  ( (filetype & SMARTGREP_FILETYPE_HEADER ) && is_header_file( p_dirent->d_name ) ) ){
			char file_name_r[512];
			strcpy( file_name_r, path );
			strcat( file_name_r, "/" );
			strcat( file_name_r, p_dirent->d_name );
			parse_file( file_name_r, wordtype, target_word ); 
		}
	}
	closedir( p_dir );	
}
#endif

bool is_source_file( char* file_name ){
	if( is_ext( file_name, "c" ) ||
		is_ext( file_name, "cpp" ) || 
		is_ext( file_name, "cxx" ) ||
		is_ext( file_name, "tli" ) ||
		is_ext( file_name, "inc" ) ||
		is_ext( file_name, "rc" ) ||
		is_ext( file_name, "m" ) ||
		is_ext( file_name, "mm" ) ||
		is_ext( file_name, "cs" ) || 
		is_ext( file_name, "js" ) ) {
		return true;
	} else {
		return false;
	}
}

bool is_header_file( char* file_name ){
	if( is_ext( file_name, "h" ) ||
		is_ext( file_name, "hpp" ) ||
		is_ext( file_name, "hxx" ) ||
		is_ext( file_name, "tlh" ) ||
		is_ext( file_name, "inl" ) ){
		return true;
	} else {
		return false;
	}
}

/*
 * @param char: ext_name "c", "cpp", "h", etc
 */
bool is_ext( char* file_name, const char* ext_name ){
	char* period = strrchr( file_name, '.' );
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

/*
 * @brief	parse one file
 * 			output to standard out
 */
void parse_file( char* file_name, int wordtype, char* target_word )
{
	// it is presumed that the one line byte size in file don't exceed 64k
	char p_data[DATASIZE]; 

	FILE* fp = fopen( file_name, "rb" );
	if( fp == NULL )
		return;

	bool c_comment = false;
	int lineno;
	for( lineno = 1; ; ++lineno ){
		memset( p_data, 0, sizeof(DATASIZE) );
		char* ptr = fgets( p_data, DATASIZE, fp );
		if( ptr == NULL )
			break;

		bool found;
		if( wordtype & SMARTGREP_WORDTYPE_EXCLUDE_COMMENT ){
			found = process_line_exclude_comment( &c_comment, p_data, wordtype, target_word );
		} else if( wordtype & SMARTGREP_WORDTYPE_INCLUDE_COMMENT ){
			found = process_line_include_comment( p_data, wordtype, target_word );
		} else {
			assert( false );
		}
		if( found ){
			printf( "%s:%d:", file_name, lineno );
			for( char* q = p_data; *q != '\r' && *q != '\n'; ++q ){
				printf( "%c", *q );
			}
			printf( "\n" );
		}
	}
	fclose( fp );
}

/*
 * @brief	parse one line in file excluding comemnt
 *
 * @retval 	true : found
 * @retval	false: not found
 *
 * @param	[in/out] 	bool* 	pc_comment 		whether in C comment
 * @param	[in]		char* 	buf
 * @param	[in]		int 	wordtype
 * @param	[in]		char* 	target_word
 */
bool process_line_exclude_comment( bool* pc_comment, char* buf, int wordtype, char* target_word )
{
	char valid_str[DATASIZE];
	memset( valid_str, 0, sizeof(valid_str) );

	int literal = 0;
	size_t i;
	char* ptr = valid_str;
	for( i = 0; i < DATASIZE; ++i ){
		if( buf[i] == '\n' || buf[i] == '\0' ) break; 
		if( !literal && !(*pc_comment) && buf[i] == '/' && buf[i+1] == '/' ){
			// C++ comment
			break;
		} else if( !literal && !(*pc_comment) && buf[i] == '/' && buf[i+1] == '*' ){
			// C comment in
			i += 2;
			*pc_comment = true;
			continue;
		} else if( !literal && *pc_comment && buf[i] == '*' && buf[i+1] == '/' ){
			i += 2;
			*pc_comment = false;
		} else if( !literal && *pc_comment && !( buf[i] == '*' && buf[i+1] == '/' ) ){
			while( true ){
				if( buf[i] == '\n' ) goto WHILEOUT;
				if( buf[i] == '*' && buf[i+1] == '/' )
					break;
				++i;
			}
			i += 2;
			*pc_comment = false;
		} else if( !*pc_comment && ( buf[i] == '\"' || buf[i] == '\'' ) && ( buf[i-1] != '\\' ) ) {
			// reverse literal
			literal = !literal;
		}
		if( buf[i] == '\r' ) break;
		if( buf[i] == '\n' || buf[i] == '\0' ) break;
		
		*ptr = buf[i];
		++ptr;
	}
WHILEOUT:
	return findword_in_line( valid_str, wordtype, target_word );
}

/*
 * @brief	search string in one line.
 * 			strstr can be hit twice in one line.
 *
 * @retval	true : found
 * @retval	false: not found
 *
 * @param	[in] char* 	valid_str
 * @param	[in] int 	wordtype
 * @param	[in] char* 	target_word
 */
bool findword_in_line( char* valid_str, int wordtype, char* target_word )
{
	if( wordtype & SMARTGREP_WORDTYPE_NORMAL ){
		// normal search
		return ( strstr( valid_str, target_word ) != NULL );
	} else if( wordtype & SMARTGREP_WORDTYPE_WORD ){
		// word search
		int   target_word_len = strlen(target_word);
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
				int   remain_str_len = strlen(remain_ptr); 
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
 * @retval	true: found
 * @retval	false:not found
 */
bool process_line_include_comment( char* buf, int wordtype, char* target_word )
{
	return findword_in_line( buf, wordtype, target_word );
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

void print_version( void )
{
#ifdef WIN32
	char szFullPath[MAX_PATH];
	GetModuleFileName( NULL, szFullPath, sizeof(szFullPath) );
	DWORD dwZero = 0;
	DWORD dwVerInfoSize = GetFileVersionInfoSize( szFullPath, &dwZero );
	if( dwVerInfoSize != 0 ){
		UCHAR* pVffInfo;
		pVffInfo = new UCHAR[dwVerInfoSize];
		if( pVffInfo == NULL )
			return;
		GetFileVersionInfo( szFullPath, dwZero, dwVerInfoSize, pVffInfo );

		void* pvVersion;
		UINT VersionLen;
		VerQueryValue( pVffInfo, TEXT("\\StringFileInfo\\041104b0\\ProductVersion"), &pvVersion, &VersionLen );

		printf( "Version %s\n", pvVersion );
		delete [] pVffInfo;
	}
#endif
}

