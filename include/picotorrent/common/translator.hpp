#pragma once

#include <map>
#include <string>
#include <vector>
#include <windows.h>

#include <picotorrent/export.hpp>

#define TR(id) picotorrent::common::translator::instance().translate(id).c_str()

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

namespace picotorrent
{
namespace common
{
    struct translation
    {
        std::string name;
        int language_id;
    };

    class translator
    {
    public:
        DLL_EXPORT static translator& instance();

        DLL_EXPORT std::vector<translation> get_available_translations();
        DLL_EXPORT int get_current_lang_id();
        DLL_EXPORT std::string translate(const std::string &key);
        DLL_EXPORT void set_current_language(int langId);

    private:
        translator();
        ~translator();

        std::string get_lang_path();

        HINSTANCE instance_;
        picojson::object strings_;
    };
}
}
