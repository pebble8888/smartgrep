/**
 * @file	smartgrep.cpp
 * @author	pebble8888@gmail.com
 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 
#endif

#include <assert.h>
#include <condition_variable>
#include <ctype.h>
#include <filesystem>
#include <memory>
#include <mutex>
#include <queue>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <time.h>
#include <vector>

#ifdef _WIN32
#else
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#endif

#include "smartgrep.h"

#ifdef _WIN32
#include <windows.h>
#endif

static constexpr int DATASIZE = 64 * 1024; // process unit size
static constexpr int DATASIZE_OUT = DATASIZE*3/sizeof(wchar_t);
static constexpr char kTab = 0x9;
static constexpr char kSpace = 0x20;

static std::queue<std::string> filenames_queue_; // use filenames_mtx_ for access
static bool done_adding_files_;   // use filenames_mtx_ for access
static std::mutex filenames_mtx_;
static std::condition_variable files_ready_cond_;
static std::mutex print_mtx_; // use for printf call
static std::vector<std::string> output_;

void test()
{
	test_is_alnum_or_underscore();
}

static void search_worker(const int wordtype, const std::string& word)
{
    while (true) {
        std::string filename;
        {
            std::unique_lock<std::mutex> lk(filenames_mtx_);
            while (filenames_queue_.empty()){
                if (done_adding_files_) {
                    lk.unlock();
                    return; // exit thread
                }
                files_ready_cond_.wait(lk);
            }

            filename = filenames_queue_.front();
            filenames_queue_.pop();
        }

        parse_file(filename.c_str(), wordtype, word.c_str());
    }
}

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	test();
#endif

	if (argc-1 < 2) {
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
	if (strcmp(argv[1], "-i") == 0) {
		info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
		wordtype |= SG_WORDTYPE_NORMAL;
		wordtype |= SG_WORDTYPE_INCLUDE_COMMENT;
	} else if (strcmp(argv[1], "-h") == 0) {
		info.filetype |= SG_FILETYPE_HEADER;
		wordtype |= SG_WORDTYPE_NORMAL;
		wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
	} else if (strcmp(argv[1], "-e") == 0) {
		info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
		wordtype |= SG_WORDTYPE_NORMAL;
		wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
	} else if (strcmp(argv[1], "-iw") == 0) {
		info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
		wordtype |= SG_WORDTYPE_WORD;
		wordtype |= SG_WORDTYPE_INCLUDE_COMMENT;
	} else if (strcmp(argv[1], "-hw") == 0) {
		info.filetype |= SG_FILETYPE_HEADER;
		wordtype |= SG_WORDTYPE_WORD;
		wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
	} else if (strcmp(argv[1], "-ew") == 0) {
		info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
		wordtype |= SG_WORDTYPE_WORD;
		wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
    } else if (strcmp(argv[1], "-c") == 0) {
        info.filetype |= (SG_FILETYPE_SOURCE|SG_FILETYPE_HEADER);
        wordtype |= SG_WORDTYPE_NORMAL;
        wordtype |= SG_WORDTYPE_INCLUDE_COMMENT;
        wordtype |= SG_WORDTYPE_CASEINSENSITIVE;
	} else {
		usage();
		return 1;
	}

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-g") == 0) {
            use_repo = true;
        } else if (strcmp(argv[i], "--nojs") == 0) {
            info.typejs = false;
        } else if (strcmp(argv[i], "--nohtml") == 0) {
            info.typehtml = false;
        } else if (strcmp(argv[i], "--ignore-dir") == 0) {
            info.foldernamelist.add_foldername(argv[i+1]);
            ++i;
        } else if (strcmp(argv[i], "--noworker") == 0) {
            use_worker = false;
        }
    }

    const std::filesystem::path basepath = [&] {
        if (use_repo) {
            // use repo folder
            return smartgrep_getrepo();
        } else {
            // user current folder
            return smartgrep_getcwd();
        }
    }();

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
    if (workers_len < 1 || !use_worker) {
        workers_len = 1;
    }
   
    done_adding_files_ = false;
    std::vector<std::shared_ptr<std::thread>> v_thread;
    for (int i = 0; i < workers_len; ++i) {
        auto th = std::make_shared<std::thread>(search_worker, wordtype, std::string(word));
        v_thread.push_back(th);
    }
    
#ifdef _WIN32
	parse_directory_win(basepath, info, wordtype, word);
#else
	parse_directory_mac(basepath, info, wordtype, word);
#endif
    
    {
        std::lock_guard<std::mutex> lk(filenames_mtx_);
        done_adding_files_ = true;
        files_ready_cond_.notify_all();
    }
    
    for (int i = 0; i < workers_len; ++i) {
        v_thread[i]->join();
    }

    std::sort(output_.begin(), output_.end());

    for (auto str: output_) {
        printf("%s", str.c_str());
    }

	return 0;
}

std::filesystem::path smartgrep_getcwd()
{
    constexpr auto size = 512;
    char buf[size];
#ifdef _WIN32
	GetCurrentDirectory(size, buf);
#else
	getcwd(buf, size);
#endif
    return std::string(buf);
}

std::filesystem::path smartgrep_getrepo()
{
    std::filesystem::path result;
    const auto curpath = smartgrep_getcwd();
    size_t idx = curpath.string().length();
    char path[512]; 
#ifdef _WIN32
	result = curpath; 
	while (idx > 3) {
		for (int i = 0; i < 2; ++i) {
            // 0:.git
            // 1:.hg
			strcpy_s(path, curpath.string().c_str());
			path[idx] = '\0';
			if (i == 0) strcat_s(path, "\\.git");
			else if (i == 1) strcat_s(path, "\\.hg");
			// check
			HANDLE h_find;
			WIN32_FIND_DATA find_data;
			h_find = FindFirstFile(path, &find_data);
			if (h_find != INVALID_HANDLE_VALUE) {
				// can access
				FindClose(h_find);
				path[idx] = '\0';
				size_t len = strlen(path);
				assert(len < size);
				return path;
			}
		}

		path[idx] = '\0';
		char* r = strrchr(path, '\\');
		if (r == nullptr) {
            return curpath;
		}

		idx = (size_t)(r - path);
	}
#else
    while (true) {
        for (int i = 0; i < 2; ++i) {
            // 0:.git
            // 1:.hg
            strcpy(path, curpath.string().c_str());
            path[idx] = '\0'; 
            if (i == 0) strcat(path, "/.git");
            else if (i == 1) strcat(path, "/.hg");
            // check
            DIR* p_dir = opendir(path);
            if (p_dir != nullptr) {
                // can access
                closedir( p_dir );
                path[idx] = '\0';
                return path;
            }
        }

        path[idx] = '\0';
        char* r = strrchr(path, '/');
        if (r == nullptr) {
            return curpath;
        }

        idx = (size_t)(r - path); 
    }
#endif
    return result;
}

void usage()
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
		"  support file extensions : .cpp/.c/.mm/.m/.h/.js/.coffee/.rb/.py/.pl/.sh/.cr/.rs/.toml\n"
        "                            .java/.scala/.go/.cs/.xaml/.resx/.vb/.bas/.frm/.cls/.pc/.cu\n"
        "                            .php/.plist/.pbxproj/.strings/.storyboard/.swift/.gyb/.vim/\n"
        "                            .css/.scss\n"
        "  asis support file extensions : .erb/.html/.xml\n"
        "\n"
        "  ignore directory : .git/.hg/.svn/.vs\n"
        "\n"
        "  Version 4.3.2\n"
	);
}

#ifdef _WIN32
/*
 @param path
 @param filetype    SG_FILETYPE_SOURCE: .c/.cpp/.m/.mm/etc
                    SG_FILETYPE_HEADER: .h/.hpp/etc
 @param wordtype	
 @param target_word
 */
void parse_directory_win(
    const std::filesystem::path& path,
    const FILE_TYPE_INFO& info,
    const int wordtype,
    const char* target_word)
{
    const auto path_name = path / "*.*";
	
	WIN32_FIND_DATA find_data;
	const auto h_find = FindFirstFile(path_name.string().c_str(), &find_data);
	if (h_find == INVALID_HANDLE_VALUE) {
		printf("directory read error! [%s]\n", path.string().c_str());
		return;
	}

	while (true) {
		if (strcmp( find_data.cFileName, ".") == 0 ||
			strcmp( find_data.cFileName, "..") == 0) {
			// do nothing
			;
		}
		else if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
		         find_data.cFileName[0] != '.') {
			// not hidden directory
            if (info.foldernamelist.has_foldername(find_data.cFileName)) {
                // ignore the folder
            } else {
                const auto path_name_r = path / find_data.cFileName; 
                parse_directory_win(path_name_r, info, wordtype, target_word);
            }
		} else if (((info.filetype & SG_FILETYPE_SOURCE) && is_source_file(info, find_data.cFileName)) ||
				   ((info.filetype & SG_FILETYPE_HEADER) && is_header_file(find_data.cFileName))) {
            // file
            const auto file_name_r = path / find_data.cFileName;
            {
                std::lock_guard<std::mutex> lk(filenames_mtx_);
                filenames_queue_.push(file_name_r.string());
                files_ready_cond_.notify_one();
            }
		}

		BOOL ret = FindNextFile(h_find, &find_data);
		if (!ret) {
			const auto dwError = GetLastError();
			assert(dwError == ERROR_NO_MORE_FILES);
			break;
		}
	}

	FindClose(h_find);
}

#else

/*
 @param path
 @param filetype    SG_FILETYPE_SOURCE: .c/.cpp/.m/.mm/.cs/.js etc
                    SG_FILETYPE_HEADER: .h/.hpp/etc
 @param wordtype
 @param target_word
 */
void parse_directory_mac(
    const std::filesystem::path& path,
    const FILE_TYPE_INFO& info,
    const int wordtype,
    const char* target_word)
{
	DIR* p_dir = opendir(path.string().c_str());
	if (p_dir == nullptr) {
		printf("directory read error! [%s]\n", path.string().c_str());
		return;
	}

	struct dirent* p_dirent;
    struct stat file_info;
	while ((p_dirent = readdir(p_dir)) != nullptr) {
        const auto path_name_r = path / p_dirent->d_name;
        lstat(path_name_r.string().c_str(), &file_info);

		if (strcmp(p_dirent->d_name, ".") == 0 ||
		    strcmp(p_dirent->d_name, "..") == 0) {
			// do nothing
			;
		} else if(S_ISDIR(file_info.st_mode) &&
				p_dirent->d_name[0] != '.') {
			// not hidden directory
            if (info.foldernamelist.has_foldername(p_dirent->d_name)) {
                // ignore the folder
            } else {
                parse_directory_mac(path_name_r, info, wordtype, target_word);
            }
		} else if (((info.filetype & SG_FILETYPE_SOURCE) && is_source_file(info, p_dirent->d_name)) ||
				  ((info.filetype & SG_FILETYPE_HEADER) && is_header_file(p_dirent->d_name))) {
            {
                std::lock_guard<std::mutex> lk(filenames_mtx_);
                filenames_queue_.push(path_name_r.string());
                files_ready_cond_.notify_one();
            }
        }
	}

	closedir(p_dir);
}
#endif

bool is_cs_file(const char* file_name) {
    return is_ext(file_name, "cs");
}

bool is_source_file(const FILE_TYPE_INFO& info, const char* file_name) {
	if (is_ext(file_name, "c") ||
		is_ext(file_name, "cpp") || 
        is_ext(file_name, "cc") ||
		is_ext(file_name, "cxx") ||
		is_ext(file_name, "tli") ||
		is_ext(file_name, "inc") ||
		is_ext(file_name, "rc") ||
		is_ext(file_name, "m") ||
		is_ext(file_name, "mm") ||
	    (is_ext(file_name, "cs") && !is_last(file_name, ".g.cs") && !is_last(file_name, ".g.i.cs")) ||
        is_ext(file_name, "xaml") ||
        is_ext(file_name, "resx") ||
		(info.typejs && is_ext(file_name, "js")) ||
		is_ext(file_name, "java") ||
        is_ext(file_name, "scala") ||
		is_ext(file_name, "go") ||
        is_ext(file_name, "rs") ||
        is_ext(file_name, "toml") ||
        is_ext(file_name, "swift") ||
        is_ext(file_name, "gyb") ||
        is_ext(file_name, "css") ||
        is_ext(file_name, "scss") ||
		is_ext(file_name, "pc") ||
        is_ext(file_name, "cu") ||
        is_ext(file_name, "php") ||
        is_shell_file(file_name) ||
        is_ruby_file(file_name) ||
        is_crystal_file(file_name) ||
        is_erb_file(file_name) ||
        (info.typehtml && is_html_file(file_name)) ||
        is_xml_file(file_name) ||
        is_coffee_file(file_name) ||
	    is_python_file(file_name) ||
        is_perl_file(file_name) ||
        is_vb_file(file_name) ||
        is_vim_file(file_name) ||
        is_xcode_resource_file(file_name)) {
		return true;
	}

	return false;
}

bool is_shell_file(const char* file_name) { return is_ext(file_name, "sh"); }
bool is_ruby_file(const char* file_name) { return is_ext(file_name, "rb"); }
bool is_crystal_file(const char* file_name) { return is_ext(file_name, "cr"); }
bool is_erb_file(const char* file_name) { return is_ext(file_name, "erb"); }
bool is_html_file(const char* file_name) {
    if (is_ext(file_name, "html") ||
        is_ext(file_name, "htm") ||
        is_ext(file_name, "css")) {
        return true;
    } else {
        return false;
    }
}

bool is_xml_file(const char* file_name) { return is_ext(file_name, "xml"); }
bool is_coffee_file(const char* file_name) { return is_ext(file_name, "coffee"); }
bool is_python_file(const char* file_name) { return is_ext(file_name, "py"); }
bool is_perl_file(const char* file_name) { return is_ext(file_name, "pl"); }
bool is_vim_file(const char* file_name) { return is_ext(file_name, "vim"); }
bool is_vb_file(const char* file_name) {
    if (is_ext(file_name, "vb") ||
        is_ext(file_name, "frm") ||
        is_ext(file_name, "bas") ||
        is_ext(file_name, "cls") ){
        return true;
    } else {
        return false;
    }
}
bool is_xcode_resource_file(const char* file_name) {
    if (is_ext(file_name, "pbxproj") ||
        is_ext(file_name, "strings") ||
        is_ext(file_name, "plist") ||
        is_ext(file_name, "storyboard")) {
        return true;
    }

    return false;
}

bool is_header_file(const char* file_name) {
	if (is_ext(file_name, "h") ||
        is_ext(file_name, "hh") ||
		is_ext(file_name, "hpp") ||
		is_ext(file_name, "hxx") ||
		is_ext(file_name, "tlh") ||
		is_ext(file_name, "inl") ||
        is_ext(file_name, "pro")) {
		return true;
	} else {
		return false;
	}
}

/*
 @param char: ext_name "c", "cpp", "h", etc
 */
bool is_ext(const char* file_name, const char* ext_name) {
	char* period = strrchr((char*)file_name, '.');
	if (period == nullptr) {
		return false;
	}

	char buf[512];
	memset(buf, 0, sizeof(buf));
	char* p;
	char* q;
	for (p = period + 1, q = buf; *p != '\0'; ++p, ++q) {
		*q = tolower(*p);
	}

	return strcmp(buf, ext_name) == 0;
}

bool is_last(const char* file_name, const char* last_name) {
    size_t i = strlen(file_name);
    size_t len = strlen(last_name);
    if (len <= 0) {
        return false;
    }

    --i;
    size_t j = len - 1;
    while (i >= 0 && j >= 0) {
        if (file_name[i] == last_name[j]) {
            if (j == 0) {
                return true;
            }
        } else {
            return false;
        }

        --i;
        --j;
    }

    return false;
}

/*
 @brief	parse one file
        output to standard out
 @param [in] const char* file_name
 @param [in] const int wordtype
 @param [in] const char* target_word
 */
void parse_file(const char* file_name, int wordtype, const char* target_word)
{
	// file extension
	int file_extension;
    if (is_shell_file(file_name) ||
        is_crystal_file(file_name)) {
        file_extension = kShell;
    } else if (is_ruby_file(file_name)) {
		file_extension = kRuby; 
    } else if (is_coffee_file(file_name)) {
        file_extension = kCoffee;
	} else if (is_python_file(file_name)) {
		file_extension = kPython;
    } else if (is_perl_file(file_name)) {
        file_extension = kPerl;
    } else if (is_vb_file(file_name)) {
        file_extension = kVB;
    } else if (is_erb_file(file_name) ||
               is_html_file(file_name) ||
               is_xml_file(file_name)) {
        file_extension = kAsIs;
    } else if (is_vim_file( file_name)) {
        file_extension = kVim;
	} else {
		file_extension = kC; 
	}

	FILE* fp = fopen(file_name, "rb");
	if (fp == nullptr)
		return;

#ifndef NOFEAT_UTF16
    bool is_utf16 = false;
    // check BOM for UTF16
    if (is_cs_file(file_name) ||
        is_xcode_resource_file(file_name)) {
        while (true) {
            int data[2];
            data[0] = fgetc(fp);
            if (feof(fp) || ferror(fp)) break;
            data[1] = fgetc(fp);
            if (feof(fp) || ferror(fp)) break;

            if (data[0] == 0xff && data[1] == 0xfe) {
                is_utf16 = true;
            }

            break;
        }

        // back to begin
        fseek(fp, 0, SEEK_SET);
    }
#endif

	auto isin_multiline_comment = false;
	Prep prep;

	// it is presumed that the one line byte size in file don't exceed 64k
    auto p_data = std::make_unique<char[]>(DATASIZE+1);
#ifndef NOFEAT_UTF16
    std::unique_ptr<char[]> q_data;
    if (is_cs_file(file_name) ||
        is_xcode_resource_file(file_name)) {
        q_data = std::make_unique<char[]>(DATASIZE_OUT+1);
    }

    int q_datasize = 0;
#endif

	int lineno;
    char* r_data = nullptr;
    size_t r_datasize = 0;
    
    const int BOMSIZE = 2;

	for (lineno = 1; ;) {
        if (r_datasize <= 0) {
            // no buffer unprocessed, so read new data.
            memset(p_data.get(), 0, DATASIZE);
            size_t sz = fread(p_data.get(), 1, DATASIZE, fp);
            if (sz <= 0) break;
#ifndef NOFEAT_UTF16
            if (is_utf16) {
                
                // number of wchar_t elements
                size_t p_datasize = (sz-BOMSIZE)/sizeof(int16_t);

                if (p_datasize == 0) break;

                memset(q_data.get(), 0, DATASIZE_OUT);
                q_datasize = UTF16LEToUTF8((int16_t*)(p_data.get()+BOMSIZE), (int)p_datasize, q_data.get());
                
                r_data = q_data.get();
                r_datasize = q_datasize;
            } else
#endif
            {
                r_data = p_data.get();
                r_datasize = sz;
            }
        }

		auto found = false;
		if (wordtype & SG_WORDTYPE_EXCLUDE_COMMENT) {
			if (file_extension == kC) {
				found = process_line_exclude_comment_c(isin_multiline_comment, prep,
													r_data, r_datasize, wordtype, target_word);
			} else if (file_extension == kShell ||
                       file_extension == kRuby ||
                       file_extension == kCoffee ||
                       file_extension == kPython ||
                       file_extension == kPerl) {
				found = process_line_exclude_comment_ruby(isin_multiline_comment,
                                                        r_data, r_datasize, wordtype, target_word,
                                                        file_extension);
            } else if (file_extension == kVB) {
                found = process_line_exclude_comment_vb(r_data, r_datasize, wordtype, target_word);
            } else if (file_extension == kVim) {
                found = process_line_exclude_comment_vim(r_data, r_datasize, wordtype, target_word);
            } else if (file_extension == kAsIs) {
                found = process_line_include_comment(r_data, r_datasize, wordtype, target_word);
			} else {
				assert(false);
			}
		} else if (wordtype & SG_WORDTYPE_INCLUDE_COMMENT) {
			found = process_line_include_comment(r_data, r_datasize, wordtype, target_word);
		} else {
			assert(false);
		}

        auto found_linebreak = false;
		if (found) {
            // found target
            char* q = r_data;
            {
                std::string outstr;
                // output filename and line number or EOF
                char buf[512];
                snprintf(buf, sizeof(buf), "%s:%d:", file_name, lineno);
                outstr = buf;
                // output until next newline or EOF
                for (; q < r_data + r_datasize; ++q) {
                    if (*q == '\n') {
                        // found unix newline
                        ++q;
                        break;
                    } else if(*q == '\r' &&
                              q+1 < r_data + r_datasize &&
                              *(q+1) == '\n') {
                        // found windows newline
                        q += 2;
                        break;
                    }
                    snprintf(buf, sizeof(buf), "%c", *q);
                    outstr += buf;
    			}

                if (q < r_data + r_datasize) {
                    found_linebreak = true;
                } else {
                    // almost EOF
                    // TODO:not a completely valid logic
                }
                outstr += "\n";
                printout(std::move(outstr));
            }

            // proceed processed buffer size
            const size_t advance = (size_t)(q-r_data);
            r_data += advance;
            r_datasize -= advance;
		} else {
            // not found target
            // proceed to newline
            char* q = r_data;
            for (; q < r_data + r_datasize; ++q) {
                if (*q == '\n') {
                    // found unit newline
                    ++q;
                    break;
                } else if (*q == '\r' &&
                           q+1 < r_data + r_datasize &&
                           *(q+1) == '\n') {
                    // found windows newline
                    q += 2;
                    break;
                }
            }
            if (q < r_data + r_datasize) {
                found_linebreak = true;
            }

            // proceed processed buffer size
            const size_t advance = (size_t)(q-r_data);
            r_data += advance;
            r_datasize -= advance;
        }

        if (found_linebreak) {
            ++lineno;
        }
	}

	fclose(fp);
}

/**
 @brief parse one line in file excluding comemnt
 @retval true : found
 @retval false: not found
 @param [in/out] isin_multiline_comment 		whether in C comment
 @param [in/out] prep
 @param [in] buf
 @param [in] bufsize
 @param [in] wordtype
 @param [in] target_word
 */
bool process_line_exclude_comment_c(
    bool& isin_multiline_comment,
    Prep& prep,
    const char* buf,
    const size_t bufsize,
    const int wordtype,
    const char* target_word)
{
	char valid_str[DATASIZE_OUT+1];

	bool isin_literal = false; // "xxx", 'xxx'
    auto p = buf;
	char* q = valid_str;
    while (p < buf + bufsize && q < valid_str + DATASIZE_OUT) {
		if (*p == '\n' || *p == '\0') break;
		if (!isin_literal && !isin_multiline_comment && !prep.is_commented()
			&& *p == '/' && *(p+1) == '/') {
			// C++ comment
			break;
		} else if (!isin_literal && !isin_multiline_comment && !prep.is_commented()
					&& *p == '/' && *(p+1) == '*') {
			// the begining of C comment
			p += 2;
			isin_multiline_comment = true;
			continue;
		} else if (!isin_literal && isin_multiline_comment && !prep.is_commented()) {
			// in c comment
			while (p < buf + bufsize) {
				if (*p == '\n' || *p == '\0') goto WHILEOUT;
				if (*p == '*' && *(p+1) == '/') {
					break;
				}
				++p;
			}
			p += 2;
			// the end of c comment
			isin_multiline_comment = false;
		} else if (!isin_multiline_comment && !prep.is_commented()
					&& ( *p == '\"' || *p == '\'') && (p == buf || *(p-1) != '\\')) {
			// reverse isin_literal
			isin_literal = !isin_literal;
		} else if (!isin_literal && !isin_multiline_comment && *p == '#') {
			if (memcmp(p, SG_PREP_IF, strlen(SG_PREP_IF)) == 0 ||
				memcmp(p, SG_PREP_IFDEF, strlen(SG_PREP_IFDEF)) == 0 ||
				memcmp(p, SG_PREP_IFNDEF, strlen(SG_PREP_IFNDEF)) == 0) {
				// #if or #ifdef or #ifndef
				if (memcmp(p, SG_PREP_IFZERO, strlen(SG_PREP_IFZERO)) == 0) {
					// #if 0
					prep.push(true);
					p += strlen(SG_PREP_IFZERO);
				} else {
					prep.push(false);
					if (memcmp(p, SG_PREP_IF, strlen(SG_PREP_IF)) == 0) {
						p += strlen(SG_PREP_IF);
					} else if (memcmp(p, SG_PREP_IFDEF, strlen(SG_PREP_IFDEF)) == 0) {
						p += strlen(SG_PREP_IFDEF);
					} else if (memcmp(p, SG_PREP_IFNDEF, strlen(SG_PREP_IFNDEF)) == 0) {
						p += strlen(SG_PREP_IFNDEF);
					} else {
						assert(false);
					}
				}

				continue;
			} else if (prep.can_change_to_else()
					   && (memcmp(p, SG_PREP_ELIF, strlen(SG_PREP_ELIF)) == 0 ||
						   memcmp(p, SG_PREP_ELSE, strlen(SG_PREP_ELSE)) == 0)) {
				prep.change_to_else();

				if (memcmp(p, SG_PREP_ELIF, strlen(SG_PREP_ELIF)) == 0) {
					p += strlen(SG_PREP_ELIF);
				} else if (memcmp(p, SG_PREP_ELSE, strlen(SG_PREP_ELSE)) == 0) {
					p += strlen(SG_PREP_ELSE);
				} else {
					assert(false);
				}

				continue;
			} else if (memcmp(p, SG_PREP_ENDIF, strlen(SG_PREP_ENDIF)) == 0) {
				// #endif
				prep.pop();
				p += strlen(SG_PREP_ENDIF);
				continue;
			}
		} else if (!isin_literal && !isin_multiline_comment && prep.is_commented()) {
            ++p;
			continue;
		}

        if (p >= buf + bufsize) {
            break;
        }

        if (*p == '\r' || *p == '\n' || *p == '\0') break;
		
		// valid data
		*(q++) = *(p++);
	}
WHILEOUT:
    *q = 0x0; // null terminate
	return findword_in_line(valid_str, wordtype, target_word);
}

/**
 dynamic languages
 */
bool process_line_exclude_comment_ruby(
    bool& isin_multiline_comment,
    const char* buf,
    const size_t bufsize,
    const int wordtype,
    const char* target_word,
    const int file_extension)
{
	char valid_str[DATASIZE_OUT];
	auto isin_dq = false; // "xxx"
	auto isin_sq = false; // 'xxx'
	auto isin_var = false; // "#{}"
	char* q = valid_str;
    for (auto p = (char*)buf; p < buf + bufsize; ) {
		if (*p == '\n' || *p == '\0') break; 

        if (!isin_multiline_comment
            && !isin_dq && !isin_sq
            && ((file_extension == kRuby && p == buf && memcmp(p, "=begin", 5) == 0) ||
                (file_extension == kPerl && p == buf && memcmp(p, "=pod", 4) == 0) ||
                (file_extension == kCoffee && memcmp(p, "###", 3) == 0) ||
                (file_extension == kPython &&
                  (memcmp(p, "\"\"\"", 3) == 0 || memcmp(p, "'''", 3) == 0))))
        {
            // the begining of multi-line comment
            if (file_extension == kRuby) { p += 5; }
            else if (file_extension == kPerl) { p += 4; }
            else if (file_extension == kCoffee) { p += 3; }
            else if (file_extension == kPython) { p += 3; }
            isin_multiline_comment = true;
            ++p;
            continue;
        } else if (!isin_dq && !isin_sq && isin_multiline_comment) {
            // in multi-line comment
            bool found = false;
            while (p < buf + bufsize) {
                if (*p == '\n') goto WHILEOUT;
                if (file_extension == kRuby && p == buf && memcmp(p, "=end", 4) == 0) {
                    p += 4;
                    found = true;
                    break;
                } else if (file_extension == kPerl && p == buf && memcmp(p, "=cut", 4) == 0) {
                    p += 4;
                    found = true;
                    break;
                } else if (file_extension == kCoffee && memcmp(p, "###", 3) == 0) {
                    p += 3;
                    found = true;
                    break;
                } else if (file_extension == kPython && 
                           (memcmp(p, "\"\"\"",3) == 0 || memcmp(p, "'''",3) == 0)) {
                    p += 3;
                    found = true;
                    break;
                }
                ++p;
            }

            if (found) {
                // the end of multi-line comment
                isin_multiline_comment = false;
            }
        } else if (!isin_multiline_comment
            && !isin_sq && !isin_dq && *p == '#') {
			// single-line comment
			break;
		} else if (!isin_multiline_comment
                   && !isin_sq && *p == '\"' && ( p == buf || *(p-1) != '\\')) {
			// reverse isin_dq
			isin_dq = !isin_dq;
		} else if (!isin_multiline_comment
                   && !isin_dq && *p == '\'' && ( p == buf || *(p-1) != '\\')) {
			// reverse isin_sq
			isin_sq = !isin_sq;
		} else if (!isin_multiline_comment
                   && isin_dq && !isin_var && *p == '#' && *(p+1) == '{') {
			isin_var = true;
		} else if (!isin_multiline_comment
                   && isin_dq && file_extension == kRuby && isin_var && *p == '}') {
			isin_var = false;
		}

		if (*p == '\r' || *p == '\n' || *p == '\0') break;
		
		// valid data
		*(q++) = *(p++);
	}

WHILEOUT:
    *q = 0x0; // null terminate
	return findword_in_line(valid_str, wordtype, target_word);
}

/**
 visual basic 6 and visual basic dot net
 */
bool process_line_exclude_comment_vb(const char* buf, const size_t bufsize, const int wordtype, const char* target_word)
{
	char valid_str[DATASIZE_OUT+1];
	auto isin_dq = false; // "xxx"
	char* q = valid_str;
    for (auto p = (char*)buf; p < buf + bufsize; ) {
		if (*p == '\n' || *p == '\0') break; 

        if (!isin_dq && *p == '\'') {
			// single-line comment
			break;
		} else if (*p == '\"' && ( p == buf || *(p-1) != '\\')) {
			// reverse isin_dq
			isin_dq = !isin_dq;
		}

		if (*p == '\r' || *p == '\n' || *p == '\0') break;
		
		// valid data
		*(q++) = *(p++);
	}

    *q = 0x0; // null terminate
	return findword_in_line(valid_str, wordtype, target_word);
}

/**
 vim script
 */
bool process_line_exclude_comment_vim(const char* buf, const size_t bufsize, const int wordtype, const char* target_word)
{
	char valid_str[DATASIZE_OUT+1];
	auto found_anything_but_whitespace = false;
    char* q = valid_str;
    for (auto p = (char*)buf; p < buf + bufsize; ) {
		if (*p == '\n' || *p == '\0' ) break;

        if (!found_anything_but_whitespace && *p == '\"') {
			// single-line comment
			break;
		}

		if (*p == '\r') break;

		if (!(*p == kSpace || *p == kTab)) found_anything_but_whitespace = true;
		
		// valid data
		*(q++) = *(p++);
	}

    *q = 0x0; // null terminate
    return findword_in_line(valid_str, wordtype, target_word);
}

/**
 @brief	search string in one line.
        strstr can be hit twice in one line.
 @retval	true : found target
 @retval	false: not found target
 @param	[in] const char* valid_str
 @param	[in] const int wordtype
 @param	[in] const char* target_word
 */
bool findword_in_line(const char* valid_str, const int wordtype, const char* target_word)
{
	if (wordtype & SG_WORDTYPE_NORMAL) {
		// normal search
        if (wordtype & SG_WORDTYPE_CASEINSENSITIVE) {
#ifdef _WIN32
            return strstr(valid_str, target_word) != nullptr;
#else
            return strcasestr(valid_str, target_word) != nullptr;
#endif
        } else {
            return strstr(valid_str, target_word) != nullptr;
        }
	} else if (wordtype & SG_WORDTYPE_WORD) {
		// word search
		const auto target_word_len = (int)strlen(target_word);
		char* remain_ptr = const_cast<char *>(valid_str);
		char* findptr = strstr(const_cast<char *>(valid_str), target_word);

		while (findptr != nullptr) {
			auto head = false;
			auto tail = false;
			if (findptr == remain_ptr) {
				head = true;
			} else {
				head = !is_alnum_or_underscore(*(findptr-1));
			}

			if (head) {
				int remain_str_len = (int)strlen(remain_ptr);
				if (remain_ptr + remain_str_len == findptr + target_word_len) {
					tail = true;
				} else {
					tail = !is_alnum_or_underscore(*(findptr + target_word_len));
				}
				if (tail) {
					return true;
				}
			}
			remain_ptr = findptr;
			findptr = strstr(findptr + target_word_len, target_word);
		}

		return false;
	}

	assert(false);
	return false;
}

/**
 @brief  as is
 @retval true: found target
 @retval false: not found target
 @param [in] buf
 @param [in] bufsize
 @param [in] worktype
 @param [in] target_word
 */
bool process_line_include_comment(const char* buf, size_t bufsize, int wordtype, const char* target_word)
{
    char valid_str[DATASIZE_OUT+1];
    char* q = valid_str;
    for (auto p = (char*)buf ; p < buf + bufsize && q < valid_str + DATASIZE_OUT;) {
        if (*p == '\0' || *p == '\r' || *p  =='\n') {
            break;
        }

        *(q++) = *(p++);
    }

    *q = 0x0; // null terminate
	return findword_in_line(valid_str, wordtype, target_word);
}

void test_is_alnum_or_underscore()
{
	assert(is_alnum_or_underscore('a'));
	assert(is_alnum_or_underscore('A'));
	assert(is_alnum_or_underscore('1'));
	assert(is_alnum_or_underscore('_'));
	assert(!is_alnum_or_underscore('%'));
}

bool is_alnum_or_underscore(int val)
{
	return isalnum(val) ? true : val == '_';	
}

/**
 @return bytelength
 */
int UTF16LEToUTF8(const int16_t* pwIn, const int count, char* pOut)
{
    int16_t* pw = const_cast<int16_t*>(pwIn);
    char* q = pOut;
    while (pw <  pwIn + count) {
        if (*pw <= 0x7f) {
            *(q++) = (char)*pw;
            if (*pw == 0x0) {
                break;
            }
        } else if (*pw <= 0x7ff) {
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

void printout(std::string&& str)
{
    std::lock_guard<std::mutex> lk(print_mtx_);
    output_.push_back(str);
}

