#include "utils.hpp"

#include <Windows.h>
#include <ShlObj.h>
#include <shlwapi.h>

namespace fs = std::experimental::filesystem;

using pt::Utils;

void Utils::openAndSelect(fs::path path)
{
    LPITEMIDLIST il = ILCreateFromPath(path.c_str());
    SHOpenFolderAndSelectItems(il, 0, 0, 0);
    ILFree(il);
}

QString Utils::ToHumanFileSize(int64_t bytes)
{
    TCHAR buf[1024];
    StrFormatByteSize64(bytes, buf, 1024);
    return QString::fromStdWString(buf);
}
