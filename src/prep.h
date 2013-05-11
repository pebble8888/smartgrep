/**
 * @file	prep.h
 * @author	pebble8888@gmail.com
 */
#ifndef PREP_H
#define PREP_H

#include <assert.h>

#define STACK_COUNT	(256) 

enum {
	SG_ST_IFONE,	/* #if 1 or #ifdef or #ifndef */
	SG_ST_IFZERO,	/* in #if 0 */
	SG_ST_ELSE,		/* in #elif or #else */
};

typedef struct {
	int status;
} COMMENT_PREP;

/**
 * @brief PREP class
 */
class PREP {
	public:
		PREP(){
			_current_depth = 0;
			for( int i = 0; i < STACK_COUNT; ++i ){
				_comment[i].status = SG_ST_IFONE;
			}
		}
		bool is_commented( void ){
			return comment_status() == SG_ST_IFZERO;
		}
		int comment_status( void ){
			return _comment[_current_depth].status;
		}
		void push( bool ifzero ){
			++_current_depth;
			if( ifzero ){
				_comment[_current_depth].status = SG_ST_IFZERO;
			} else {	
				_comment[_current_depth].status = SG_ST_IFONE;
			}
		}
		// not already in SG_ST_ELSE
		bool can_change_to_else( void ){
			return comment_status() == SG_ST_IFZERO ||
				   comment_status() == SG_ST_IFONE;
		}
		void change_to_else( void ){
			_comment[_current_depth].status = SG_ST_ELSE;
		}
		void pop( void ){
			_comment[_current_depth].status = SG_ST_IFONE;
			--_current_depth;
		}
	private:
		int 		 _current_depth;
		COMMENT_PREP _comment[STACK_COUNT];
};

#endif /* PREP_H */

