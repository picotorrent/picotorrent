#pragma once

#include <string>

namespace pico
{
    class Util
    {
    public:
        static std::string ToString(const std::wstring val);
        static std::wstring ToWideString(const std::string val);
        static std::wstring ToFileSize(int64_t size);
        static std::wstring ToPriority(int prio);
        static std::wstring ToSpeed(int64_t rate);
        static std::wstring ToState(int state);
    };
}
