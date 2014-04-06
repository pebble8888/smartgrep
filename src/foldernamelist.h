#include <vector>
#include <string>

class Foldernamelist {
public:
    Foldernamelist();
    ~Foldernamelist();
    bool has_foldername(const char* foldername);
    void add_foldername(const char* foldername);
private:
    std::vector<std::string> v_foldername;
};

