#include "foldernamelist.h"
#include <assert.h>
#include <string.h>

Foldernamelist::Foldernamelist()
{
    foldernames_.insert(std::string(".vs"));
    foldernames_.insert(std::string(".svn"));
    foldernames_.insert(std::string(".git"));
    foldernames_.insert(std::string(".hg"));
}

bool Foldernamelist::has_foldername(const char* foldername) const
{
    const auto name = std::string(foldername);
    return foldernames_.contains(name);
}

void Foldernamelist::add_foldername(const char* foldername)
{
    if (strlen(foldername) == 0) {
        assert(false);
    }

    foldernames_.insert(std::string(foldername));
}

