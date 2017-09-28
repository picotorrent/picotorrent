#include "utils.hpp"

#include <Windows.h>
#include <Shlwapi.h>

namespace pt
{
    std::wstring ToHumanFileSize(int64_t bytes)
    {
		TCHAR buffer[1024];
		StrFormatByteSize64(bytes, buffer, ARRAYSIZE(buffer));
		return buffer;
    }
}
