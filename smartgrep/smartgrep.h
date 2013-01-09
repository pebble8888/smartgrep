/**
 *	@file 	smartgrep.h
 *	@author	pebble8888@gmail.com
 */

#ifndef SMARTGREP_H
#define SMARTGREP_H


#define SG_WORDTYPE_WORD			0x01
#define SG_WORDTYPE_NORMAL			0x02
#define SG_WORDTYPE_INCLUDE_COMMENT	0x04
#define SG_WORDTYPE_EXCLUDE_COMMENT	0x08

#define SG_FILETYPE_HEADER	0x01	// .h/.hpp/etc 
#define SG_FILETYPE_SOURCE	0x02	// .c/.cpp/.m/.mm/.cs/.js etc

/* preprocessor comment */
#define SG_PREP_IFZERO	"#if 0"
#define SG_PREP_IF		"#if"
#define SG_PREP_ELIF	"#elif"
#define SG_PREP_ELSE 	"#else"
#define SG_PREP_ENDIF	"#endif"

enum {
	SG_ST_NOTIN,	/* not in #if 0 */
	SG_ST_IFZERO,	/* in #if 0 */
	SG_ST_ELSE,		/* in #elif or #else */
};

#include "prep.h"

/**
 * @brief	function prototype
 */
void usage( void );
void parse_file( char* file_name, int wordtype, char* target_word );
bool process_line_exclude_comment( 	bool* p_isin_c_comment, PREP* p_prep,
									char* buf, size_t bufsize, int wordtype, char* target_word );
bool process_line_include_comment( char* buf, int wordtype, char* target_word );
#ifdef WIN32
void parse_directory_win( char* path, int filetype, int wordtype, char* target_word );
#else
void parse_directory_mac( char* path, int filetype, int wordtype, char* target_word );
#endif
bool is_header_file( char* file_name );
bool is_source_file( char* file_name );
bool is_ext( char* file_name, const char* ext_name );
bool findword_in_line( char* valid_str, int wordtype, char* target_word );
bool is_alnum_or_underscore( int val );
void print_version( void );

void smartgrep_getcwd( char* buf, size_t size );

void test_is_alnum_or_underscore( void );
void test( void );

#endif /* SMARTGREP_H */

