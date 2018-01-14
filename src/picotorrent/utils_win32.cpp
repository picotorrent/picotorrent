#include "utils_win32.hpp"

#include <Windows.h>
#include <ShlObj.h>

namespace fs = std::experimental::filesystem::v1;
using pt::Utils_Win32;

void Utils_Win32::OpenAndSelect(fs::path path)
{
    LPITEMIDLIST il = ILCreateFromPath(path.c_str());
    SHOpenFolderAndSelectItems(il, 0, 0, 0);
    ILFree(il);
}

