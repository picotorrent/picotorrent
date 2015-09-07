#include "file.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace pico::io;

void File::ReadBuffer(const std::string& path, std::vector<char>& buffer)
{
    std::ifstream input(path, std::ios::binary);

    uintmax_t s = fs::file_size(path);
    buffer.resize(s, 0);
    buffer.assign(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());
}

void File::ReadBuffer(const std::wstring& path, std::vector<char>& buffer)
{
    std::ifstream input(path, std::ios::binary);

    uintmax_t s = fs::file_size(path);
    buffer.resize(s, 0);
    buffer.assign(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());
}

void File::WriteBuffer(const std::string& path, const std::vector<char>& buffer)
{
    std::ofstream output(path, std::ios::binary);
    output.write(&buffer[0], buffer.size());
    output.flush();
}

void File::WriteBuffer(const std::wstring& path, const std::vector<char>& buffer)
{
    std::ofstream output(path, std::ios::binary);
    output.write(&buffer[0], buffer.size());
    output.flush();
}
