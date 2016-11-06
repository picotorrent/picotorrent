#include "Translator.hpp"

#include <picojson.hpp>
#include <sstream>

#include "Configuration.hpp"
#include "Environment.hpp"
#include "IO/Directory.hpp"
#include "IO/File.hpp"
#include "IO/Path.hpp"

#include "resources.h"

namespace pj = picojson;

Translator::Translator()
    : m_instance(GetModuleHandle(NULL))
{
    Configuration &cfg = Configuration::GetInstance();

    std::wstringstream ss;
    ss << cfg.GetCurrentLanguageId() << TEXT(".json");
    std::wstring lang_file = IO::Path::Combine(GetLanguagePath(), ss.str());

    if (IO::File::Exists(lang_file))
    {
        std::error_code ec;
        std::vector<char> buf = IO::File::ReadAllBytes(lang_file, ec);

        pj::value v;
        pj::parse(v, buf.begin(), buf.end());

        m_strings = v.get<pj::object>()["strings"].get<pj::object>();
    }
    else
    {
        // Load JSON from our resource
        HRSRC rc = FindResource(
            m_instance,
            MAKEINTRESOURCE(1337),
            MAKEINTRESOURCE(TEXTFILE));

        HGLOBAL data = LoadResource(m_instance, rc);
        std::string json = static_cast<const char*>(LockResource(data));

        pj::value v;
        pj::parse(v, json);

        m_strings = v.get<pj::object>()["strings"].get<pj::object>();
    }
}

Translator::~Translator()
{
}

Translator& Translator::GetInstance()
{
    static Translator instance;
    return instance;
}

std::vector<Translator::Language> Translator::GetAvailableLanguages()
{
    std::wstring lang_path = GetLanguagePath();

    Language def{ "English (United States)", 1033 };
    std::vector<Language> langs;
    langs.push_back(def);

    if (!IO::Directory::Exists(lang_path))
    {
        return langs;
    }

    for (std::wstring &path : IO::Directory::GetFiles(lang_path, TEXT("*.json")))
    {
        std::error_code ec;
        std::vector<char> buf = IO::File::ReadAllBytes(path, ec);

        if (ec)
        {
            // LOG
            continue;
        }

        pj::value v;
        std::string err = pj::parse(v, buf.begin(), buf.end());

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

        Language l;
        l.code = (int)obj.at("lang_id").get<int64_t>();
        l.name = name;

        if (l.code == def.code)
        {
            continue;
        }

        langs.push_back(l);
    }

    return langs;
}

std::string Translator::Translate(const std::string& key)
{
    std::string result = key;

    if (m_strings.find(key) != m_strings.end())
    {
        result = m_strings[key].get<std::string>();
    }

    return result;
}

std::wstring Translator::GetLanguagePath()
{
    return IO::Path::Combine(Environment::GetApplicationPath(), TEXT("lang"));
}
