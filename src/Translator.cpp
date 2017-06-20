#include "Translator.hpp"

#include <picojson.hpp>
#include <sstream>

#include "Configuration.hpp"
#include "Environment.hpp"
#include "resources.h"

namespace pj = picojson;

Translator::Translator()
{
    EnumResourceNames(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(LANGFILE),
        LoadTranslations,
        reinterpret_cast<LONG_PTR>(this));

    Configuration &cfg = Configuration::GetInstance();
    m_currentLanguage = cfg.GetCurrentLanguageId();

    if (m_strings.find(m_currentLanguage) == m_strings.end())
    {
        m_currentLanguage = 1033;
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
    return m_languages;
}

std::string Translator::Translate(const std::string& key)
{
    std::string result = key;

    if (m_strings.find(m_currentLanguage) == m_strings.end())
    {
        return result;
    }

    std::map<std::string, std::string>& str = m_strings.at(m_currentLanguage);

    if (str.find(key) == str.end())
    {
        return result;
    }

    return str.at(key);
}

BOOL Translator::LoadTranslations(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
    Translator* tr = reinterpret_cast<Translator*>(lParam);

    HRSRC rc = FindResource(hModule, lpszName, lpszType);
    HGLOBAL data = LoadResource(hModule, rc);
    std::string json = static_cast<const char*>(LockResource(data));

    pj::value v;
    std::string err = pj::parse(v, json);

    if (!err.empty())
    {
        return TRUE;
    }

    pj::object obj = v.get<pj::object>();

    int langId = static_cast<int>(obj.at("lang_id").get<int64_t>());
    std::string langName = obj.at("lang_name").get<std::string>();

    Language l{ langName, langId };
    tr->m_languages.push_back(l);

    if (tr->m_strings.find(langId) == tr->m_strings.end())
    {
        tr->m_strings.insert({ langId, std::map<std::string, std::string>() });
    }

    for (auto& p : obj.at("strings").get<pj::object>())
    {
        tr->m_strings.at(langId).insert({ p.first, p.second.get<std::string>() });
    }

    return TRUE;
}
