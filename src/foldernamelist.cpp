#include "foldernamelist.h"
#include <assert.h>
#include <string.h>

Foldernamelist::Foldernamelist()
{
    foldernames_.push_back(std::string(".vs"));
    foldernames_.push_back(std::string(".svn"));
    foldernames_.push_back(std::string(".git"));
    foldernames_.push_back(std::string(".hg"));
}

bool Foldernamelist::has_foldername(const char* foldername) const
{
    for (auto it: foldernames_) {
        if (strcmp(it.c_str(), foldername) == 0) {
            return true;
        }
    }
    return false;
}

void Foldernamelist::add_foldername(const char* foldername)
{
    if (strlen(foldername) == 0) {
        assert(false);
    }
    foldernames_.push_back(std::string(foldername));
}

