#include "fsutil.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

std::string FsUtil::GetDataPath()
{
    fs::path p("C:/ProgramData/PicoTorrent");

    if (!fs::exists(p))
    {
        fs::create_directories(p);
    }

    return p.string();
}

void FsUtil::ReadFile(std::string path, std::vector<char>& buffer)
{
    uintmax_t size = fs::file_size(path);
    
    std::ifstream input(path, std::ios::binary);
    buffer.reserve(size);

    buffer.assign(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());
}
