#include <vector>
#include <string>

#pragma once

class Foldernamelist {
public:
    Foldernamelist();
    bool has_foldername(const char* foldername) const;
    void add_foldername(const char* foldername);
private:
    std::vector<std::string> foldernames_;
};

