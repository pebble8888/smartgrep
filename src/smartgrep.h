/**
 *	@file 	smartgrep.h
 *	@author	pebble8888@gmail.com
 */

#pragma once

#include <filesystem>

#define SG_WORDTYPE_WORD			(1<<0)
#define SG_WORDTYPE_NORMAL			(1<<1)
#define SG_WORDTYPE_CASEINSENSITIVE (1<<2)
#define SG_WORDTYPE_INCLUDE_COMMENT	(1<<3)
#define SG_WORDTYPE_EXCLUDE_COMMENT	(1<<4)

#define SG_FILETYPE_HEADER	(1<<0)	// .h/.hpp/etc 
#define SG_FILETYPE_SOURCE	(1<<1)	// .c/.cpp/.m/.mm/.cs/.js etc

/* preprocessor comment */
#define SG_PREP_IFZERO	"#if 0"
#define SG_PREP_IF		"#if " // don't hit #ifdef or #ifndef
#define SG_PREP_IFDEF	"#ifdef"
#define SG_PREP_IFNDEF	"#ifndef"
#define SG_PREP_ELIF	"#elif"
#define SG_PREP_ELSE 	"#else"
#define SG_PREP_ENDIF	"#endif"

/* file extension */
enum {
	kC,
    kShell,
	kRuby,
    kCoffee,
	kPython,
    kPerl,
    kVB,
    kVim, 
    kAsIs,
};

#include "foldernamelist.h"
#include "prep.h"

/**
 * @brief command option
 */
typedef struct {
    int filetype;
    bool typejs;
    bool typehtml;
    Foldernamelist foldernamelist;
} FILE_TYPE_INFO;

/**
 * @brief	function prototype
 */
void usage(void);
void parse_file(const char* file_name, int wordtype, const char* target_word);
bool process_line_exclude_comment_c(bool& isin_multiline_comment, Prep& prep, const char* buf, size_t bufsize, int wordtype, const char* target_word);
bool process_line_exclude_comment_ruby(bool& isin_multiline_comment, const char* buf, size_t bufsize, int wordtype, const char* target_word, int file_extension);
bool process_line_exclude_comment_vb(const char* buf, size_t bufsize, int wordtype, const char* target_word); 
bool process_line_exclude_comment_vim(const char* buf, size_t bufsize, int wordtype, const char* target_word); 
bool process_line_include_comment(const char* buf, size_t bufsize, int wordtype, const char* target_word);
#ifdef _WIN32
void parse_directory_win(const std::filesystem::path& path, const FILE_TYPE_INFO& info, int wordtype, const char* target_word);
#else
void parse_directory_mac(const std::filesystem::path& path, const FILE_TYPE_INFO& info, int wordtype, const char* target_word);
#endif
bool is_header_file(const char* file_name);
bool is_cs_file(const char* file_name);
bool is_source_file(const FILE_TYPE_INFO& info, const char* file_name);
bool is_shell_file(const char* file_name);
bool is_ruby_file(const char* file_name);
bool is_crystal_file(const char* file_name);
bool is_erb_file(const char* file_name);
bool is_html_file(const char* file_name);
bool is_xml_file(const char* file_name);
bool is_coffee_file(const char* file_name);
bool is_python_file(const char* file_name);
bool is_perl_file(const char* file_name);
bool is_vb_file(const char* file_name);
bool is_vim_file(const char* file_name);
bool is_xcode_resource_file(const char* file_name);
bool is_ext(const char* file_name, const char* ext_name);
bool is_last(const char* file_name, const char* last_name);
bool findword_in_line(char* valid_str, int wordtype, const char* target_word);
bool is_alnum_or_underscore(int val);
void print_version(void);
int UTF16LEToUTF8(int16_t* pIn, int count, char* pOut);

std::filesystem::path smartgrep_getcwd();
std::filesystem::path smartgrep_getrepo();
void test_is_alnum_or_underscore(void);
void test(void);

