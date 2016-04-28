/**
 * @file	prep.h
 * @author	pebble8888@gmail.com
 */
#ifndef PREP_H
#define PREP_H

#include <assert.h>
#include <stack>

#define STACK_COUNT	(256) 

enum {
	SG_ST_IFONE,	/* #if 1 or #ifdef or #ifndef */
	SG_ST_IFZERO,	/* in #if 0 */
	SG_ST_ELSE,		/* in #elif or #else */
};

struct COMMENT_PREP {
	int status = SG_ST_IFONE;
};

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
            assert(_current_depth <= STACK_COUNT);
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
        int depth(void){
            return _current_depth;
        }
	private:
		int 		 _current_depth;
		COMMENT_PREP _comment[STACK_COUNT];
};
/*
class PREP {
public:
	PREP(){
        COMMENT_PREP comment_prep;
        _comment.push(comment_prep);
	}
	bool is_commented( void ){
		return comment_status() == SG_ST_IFZERO;
	}
	int comment_status( void ){
        return _comment.top().status;
	}
	void push( bool ifzero ){
        COMMENT_PREP comment_prep;
        _comment.push(comment_prep);
		if( ifzero ){
            _comment.top().status = SG_ST_IFZERO;
		} else {	
            _comment.top().status = SG_ST_IFONE;
		}
	}
	// not already in SG_ST_ELSE
	bool can_change_to_else( void ){
		return comment_status() == SG_ST_IFZERO ||
			   comment_status() == SG_ST_IFONE;
	}
	void change_to_else( void ){
        _comment.top().status = SG_ST_ELSE;
	}
	void pop( void ){
        _comment.pop();
	}
private:
    std::stack<COMMENT_PREP> _comment;
};
 */

#endif /* PREP_H */

