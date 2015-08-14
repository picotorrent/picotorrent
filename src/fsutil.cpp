#include "fsutil.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

void FsUtil::ReadFile(std::string path, std::vector<char>& buffer)
{
    uintmax_t size = fs::file_size(path);
    
    std::ifstream input(path, std::ios::binary);
    buffer.reserve(size);

    buffer.assign(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());
}
