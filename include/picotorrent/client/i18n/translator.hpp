#pragma once

#include <map>
#include <string>
#include <vector>
#include <windows.h>

#define TR(id) picotorrent::client::i18n::translator::instance().translate(id).c_str()

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

namespace picotorrent
{
namespace client
{
namespace i18n
{
    struct translation
    {
        std::string name;
        int language_id;
    };

    class translator
    {
    public:
        static translator& instance();

        std::vector<translation> get_available_translations();
        int get_current_lang_id();
        std::string translate(const std::string &key);
        void set_current_language(int langId);

    private:
        translator();
        ~translator();

        std::string get_lang_path();

        HINSTANCE instance_;
        picojson::object strings_;
    };
}
}
}
