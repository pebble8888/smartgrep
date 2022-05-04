/**
 * @file	prep.h
 * @author	pebble8888@gmail.com
 */
#pragma once

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

class Prep {
public:
    Prep();
    bool is_commented() const;
    int comment_status() const;
    void push(bool ifzero);
    // not already in SG_ST_ELSE
    bool can_change_to_else() const;
    void change_to_else();
    void pop();
    int depth() const;
private:
    int _current_depth {};
    COMMENT_PREP _comment[STACK_COUNT] {};
};
