#include <string>
#include <unordered_set>

#pragma once

class Foldernamelist {
public:
    Foldernamelist();
    bool has_foldername(const char* foldername) const;
    void add_foldername(const char* foldername);
private:
    std::unordered_set<std::string> foldernames_;
};

