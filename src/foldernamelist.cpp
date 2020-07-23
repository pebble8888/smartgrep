#include "foldernamelist.h"
#include <assert.h>
#include <string.h>

Foldernamelist::Foldernamelist()
{
    v_foldername.push_back(std::string(".vs"));
    v_foldername.push_back(std::string(".svn"));
    v_foldername.push_back(std::string(".git"));
    v_foldername.push_back(std::string(".hg"));
}

Foldernamelist::~Foldernamelist()
{
}

bool Foldernamelist::has_foldername(const char* foldername)
{
    for (auto it: v_foldername) {
        if (strcmp( it.c_str(), foldername) == 0) {
            return true;
        }
    }
    return false;
}

void Foldernamelist::add_foldername(const char* foldername)
{
    if (strlen(foldername) == 0) {
        assert( false );
    }
    v_foldername.push_back(std::string(foldername));
}

