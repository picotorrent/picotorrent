#include "utils.hpp"

#include <Windows.h>
#include <shlwapi.h>

using pt::Utils;

QString Utils::ToHumanFileSize(int64_t bytes)
{
    TCHAR buf[1024];
    StrFormatByteSize64(bytes, buf, 1024);
    return QString::fromStdWString(buf);
}
