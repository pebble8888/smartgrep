#include "prep.h"

Prep::Prep()
{
    _current_depth = 0;
    for (int i = 0; i < STACK_COUNT; ++i) {
        _comment[i].status = SG_ST_IFONE;
    }
}

bool Prep::is_commented() const
{
    return comment_status() == SG_ST_IFZERO;
}

int Prep::comment_status() const
{
    return _comment[_current_depth].status;
}

void Prep::push(bool ifzero)
{
    ++_current_depth;
    assert(_current_depth <= STACK_COUNT);
    if (ifzero) {
        _comment[_current_depth].status = SG_ST_IFZERO;
    } else {	
        _comment[_current_depth].status = SG_ST_IFONE;
    }
}

bool Prep::can_change_to_else() const
{
    return comment_status() == SG_ST_IFZERO ||
           comment_status() == SG_ST_IFONE;
}

void Prep::change_to_else(void)
{
    _comment[_current_depth].status = SG_ST_ELSE;
}

void Prep::pop()
{
    _comment[_current_depth].status = SG_ST_IFONE;
    --_current_depth;
}

int Prep::depth() const
{
    return _current_depth;
}
