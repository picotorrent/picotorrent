#include <picotorrent/client/i18n/translator.hpp>

#include <picojson.hpp>
#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/core/pal.hpp>

#include <fstream>
#include <sstream>

#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>

#define TEXTFILE 256

namespace pj = picojson;
using picotorrent::client::to_wstring;
using picotorrent::client::configuration;
using picotorrent::client::i18n::translation;
using picotorrent::client::i18n::translator;
using picotorrent::core::pal;

translator::translator()
    : instance_(GetModuleHandle(NULL))
{
    configuration &cfg = configuration::instance();

    std::stringstream ss;
    ss << cfg.current_language_id() << ".json";
    std::string lang_file = pal::combine_paths(get_lang_path(), ss.str());

    if (pal::file_exists(lang_file))
    {
        std::ifstream input(lang_file, std::ios::binary);

        pj::value v;
        pj::parse(v, input);

        strings_ = v.get<pj::object>()["strings"].get<pj::object>();
    }
    else
    {
        // Load JSON from our resource
        HRSRC rc = FindResource(instance_, MAKEINTRESOURCE(1337), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL data = LoadResource(instance_, rc);
        std::string json = static_cast<const char*>(LockResource(data));

        pj::value v;
        pj::parse(v, json);

        strings_ = v.get<pj::object>()["strings"].get<pj::object>();
    }
}

translator::~translator()
{
}

translator& translator::instance()
{
    static translator instance;
    return instance;
}

std::vector<translation> translator::get_available_translations()
{
    std::string lang_path = get_lang_path();

    translation def{ "English (United States)", 1033 };

    std::vector<translation> langs;
    langs.push_back(def);

    if (!pal::directory_exists(lang_path))
    {
        return langs;
    }

    for (std::string &path : pal::get_directory_entries(lang_path, "*.json"))
    {
        std::ifstream input(path, std::ios::binary);

        pj::value v;
        std::string err = pj::parse(v, input);

        if (!err.empty())
        {
            // TODO: log
            continue;
        }

        if (!v.is<pj::object>())
        {
            // TODO: log
            continue;
        }

        pj::object obj = v.get<pj::object>();

        if (obj.find("lang_name") == obj.end())
        {
            // TODO: log
            continue;
        }

        std::string name = obj.at("lang_name").get<std::string>();

        translation t;
        t.language_id = (int)obj.at("lang_id").get<int64_t>();
        t.name = name;

        if (t.language_id == def.language_id)
        {
            continue;
        }

        langs.push_back(t);
    }

    return langs;
}

int translator::get_current_lang_id()
{
    configuration &cfg = configuration::instance();
    return cfg.current_language_id();
}

std::string translator::translate(const std::string &key)
{
    std::string result = key;

    if (strings_.find(key) != strings_.end())
    {
        result = strings_[key].get<std::string>();
    }

    return result;
}

void translator::set_current_language(int langId)
{
    configuration &cfg = configuration::instance();
    cfg.set_current_language_id(langId);
}

std::string translator::get_lang_path()
{
    TCHAR path[MAX_PATH];

    // Get directory where PicoTorrent.exe resides
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);
    PathCombine(path, path, L"lang");

    return to_string(path);
}
