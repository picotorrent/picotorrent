#include <picotorrent/i18n/translator.hpp>

#include <picotorrent/picojson.hpp> 
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/config/configuration.hpp>
#include <picotorrent/filesystem/file.hpp>
#include <picotorrent/filesystem/path.hpp>

#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>

#define TEXTFILE 256

namespace fs = picotorrent::filesystem;
namespace pj = picojson;
using picotorrent::common::to_wstring;
using picotorrent::config::configuration;
using picotorrent::i18n::translator;

translator::translator()
    : instance_(GetModuleHandle(NULL))
{
    configuration &cfg = configuration::instance();

    TCHAR path[MAX_PATH];
    TCHAR file[1024];

    // Get directory where PicoTorrent.exe resides
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);

    // Get the path to our language DLL.
    StringCchPrintf(file, ARRAYSIZE(file), L"%d.json", cfg.current_language_id());

    PathCombine(path, path, L"lang");
    PathCombine(file, path, file);

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
        DWORD size = SizeofResource(instance_, rc);
        std::string json = static_cast<const char*>(LockResource(data));
        
        pj::value v;
        pj::parse(v, json);

        strings_ = v.get<pj::object>()["strings"].get<pj::object>();
    }
}

translator::~translator()
{
}

std::wstring translator::translate(const std::string &key)
{
    static translator instance;
    return instance.get_string(key);
}

std::wstring translator::get_string(const std::string &key)
{
    std::string result = key;

    if (strings_.find(key) != strings_.end())
    {
        result = strings_[key].get<std::string>();
    }

    return to_wstring(result);
}
