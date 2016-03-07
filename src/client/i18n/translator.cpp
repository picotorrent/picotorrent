#include <picotorrent/client/i18n/translator.hpp>

#include <picojson.hpp>
#include <picotorrent/core/string_operations.hpp>
#include <picotorrent/core/configuration.hpp>
#include <picotorrent/core/filesystem/file.hpp>
#include <picotorrent/core/filesystem/directory.hpp>
#include <picotorrent/core/filesystem/path.hpp>

#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>

#define TEXTFILE 256

namespace fs = picotorrent::core::filesystem;
namespace pj = picojson;
using picotorrent::core::to_wstring;
using picotorrent::core::configuration;
using picotorrent::client::i18n::translation;
using picotorrent::client::i18n::translator;

translator::translator()
    : instance_(GetModuleHandle(NULL))
{
    configuration &cfg = configuration::instance();

    TCHAR file[1024];
    StringCchPrintf(file, ARRAYSIZE(file), L"%d.json", cfg.current_language_id());
    PathCombine(file, get_lang_path().c_str(), file);

    DWORD dwAttrib = GetFileAttributes(file);

    if (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        // Load JSON from file
        fs::file f(file);
        std::vector<char> buf;
        f.read_all(buf);

        pj::value v;
        pj::parse(v, std::string(buf.begin(), buf.end()));

        strings_ = v.get<pj::object>()["strings"].get<pj::object>();
    }
    else
    {
        // Load JSON from our resource
        HRSRC rc = FindResource(instance_, MAKEINTRESOURCE(1337), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL data = LoadResource(instance_, rc);
        //DWORD size = SizeofResource(instance_, rc);
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
    fs::path p = get_lang_path();
    fs::directory d(p);

    translation def;
    def.language_id = 1033;
    def.name = L"English (United States)";

    std::vector<translation> langs;
    langs.push_back(def);

    if (!d.path().exists())
    {
        return langs;
    }

    for (fs::path &path : d.get_files(d.path().combine(L"*.json")))
    {
        std::vector<char> buf;
        fs::file(path).read_all(buf);

        pj::value v;
        std::string err = pj::parse(v, std::string(buf.begin(), buf.end()));

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
        t.name = to_wstring(name);

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

std::wstring translator::translate(const std::string &key)
{
    std::string result = key;

    if (strings_.find(key) != strings_.end())
    {
        result = strings_[key].get<std::string>();
    }

    return to_wstring(result);
}

void translator::set_current_language(int langId)
{
    configuration &cfg = configuration::instance();
    cfg.set_current_language_id(langId);
}

std::wstring translator::get_lang_path()
{
    TCHAR path[MAX_PATH];

    // Get directory where PicoTorrent.exe resides
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);
    PathCombine(path, path, L"lang");

    return path;
}
