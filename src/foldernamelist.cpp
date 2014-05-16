#include "foldernamelist.h"
#include <assert.h>
#include <string.h>

Foldernamelist::Foldernamelist()
{
}

Foldernamelist::~Foldernamelist()
{
}

bool Foldernamelist::has_foldername(const char* foldername)
{
    std::vector<std::string>::iterator it;
    for( it = v_foldername.begin(); it != v_foldername.end(); ++it ){
        if( strcmp( (*it).c_str(), foldername ) == 0 ){
            return true;
        }
    }
    return false;
}

void Foldernamelist::add_foldername(const char* foldername)
{
    if( strlen(foldername) == 0 ) {
        assert( false );
    }
    v_foldername.push_back( std::string( foldername ) );
}

