#include "utils.hpp"

#include <Windows.h>
#include <Shlwapi.h>

using pt::Utils;

std::wstring Utils::ToHumanFileSize(int64_t bytes)
{
	TCHAR buffer[1024];
	StrFormatByteSize64(bytes, buffer, ARRAYSIZE(buffer));
	return buffer;
}