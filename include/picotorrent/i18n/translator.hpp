#pragma once

#include <map>
#include <string>
#include <windows.h>

#define TR(id) picotorrent::i18n::translator::translate(id).c_str()

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

namespace picotorrent
{
namespace i18n
{
    class translator
    {
    public:
        static std::wstring translate(const std::string &key);

    private:
        translator();
        ~translator();

        std::wstring get_string(const std::string &key);

        HINSTANCE instance_;
        picojson::object strings_;
    };
}
}
