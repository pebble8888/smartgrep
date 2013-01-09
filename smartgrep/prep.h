/**
 * @file	prep.h
 * @author	pebble8888@gmail.com
 */
#ifndef PREP_H
#define PREP_H

#include <assert.h>

#define STACK_COUNT	(256) 

typedef struct {
	int status;
	int depth;
} COMMENT_PREP;

/**
 * @brief PREP class
 */
struct PREP {
	int current_depth;
	int comment_index;
	COMMENT_PREP comment[STACK_COUNT];
	PREP(){
		current_depth = 0;
		comment_index = 0;
		for( int i = 0; i < STACK_COUNT; ++i ){
			comment[i].status = SG_ST_NOTIN;
			comment[i].depth = 0;
		}
	}
	bool is_commented( void ){
		return comment_status() == SG_ST_IFZERO;
	}
	int comment_status( void ){
		return comment[comment_index].status;
	}
	int comment_depth( void ){
		return comment[comment_index].depth;
	}
	void push( void ){
		if( comment_status() == SG_ST_NOTIN ){
			comment[comment_index].status = SG_ST_IFZERO;
			comment[comment_index].depth = current_depth;
		} else if( comment_status() == SG_ST_ELSE ){
			++comment_index;
			comment[comment_index].status = SG_ST_IFZERO;
			comment[comment_index].depth = current_depth;
		} else if( comment_status() == SG_ST_IFZERO ){
			assert( false );
		}
	}
	bool can_change_to_else( void ){
		return comment_status() == SG_ST_IFZERO
				&& comment_depth() == current_depth;
	}
	void change_to_else( void ){
		comment[comment_index].status = SG_ST_ELSE;
	}
	void pop( void ){
		if( comment_depth() == current_depth ){
			comment[comment_index].status = SG_ST_NOTIN;
			comment[comment_index].depth = 0;
			--comment_index;
		}
	}
};

#endif /* PREP_H */

