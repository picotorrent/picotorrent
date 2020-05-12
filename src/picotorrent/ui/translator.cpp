#include "translator.hpp"

#include <loguru.hpp>

#include "../core/utils.hpp"
#include "../picojson.hpp"

namespace pj = picojson;
using pt::UI::Translator;

Translator::Translator()
{
}

Translator& Translator::GetInstance()
{
    static Translator translator;
    return translator;
}

void Translator::LoadEmbedded(HINSTANCE hInstance)
{
    EnumResourceNames(
        hInstance,
        TEXT("LANGFILE"),
        EnumLanguageFiles,
        reinterpret_cast<LONG_PTR>(this));

    LOG_F(INFO, "Found %d embedded translation files", m_languages.size());
}

BOOL Translator::EnumLanguageFiles(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
    Translator* translator = reinterpret_cast<Translator*>(lParam);

    HRSRC rc = FindResource(hModule, lpszName, lpszType);
    DWORD size = SizeofResource(hModule, rc);
    HGLOBAL data = LoadResource(hModule, rc);
    const char* buffer = reinterpret_cast<const char*>(LockResource(data));

    std::string json(buffer, static_cast<size_t>(size));

    pj::value v;
    std::string err = pj::parse(v, json);

    if (!err.empty())
    {
        if (IS_INTRESOURCE(lpszName))
        {
            LOG_F(ERROR, "Failed to parse language json (%d): %s", reinterpret_cast<int>(lpszName), err.data());
        }
        else
        {
            LOG_F(ERROR, "Failed to parse language json: %s", err.data());
        }

        return TRUE;
    }

    pj::object obj = v.get<pj::object>();

    int langId = static_cast<int>(obj.at("lang_id").get<int64_t>());
    std::string langName = obj.at("lang_name").get<std::string>();

    Language l;
    l.code = langId;
    l.name = Utils::toStdWString(langName);

    for (auto& p : obj.at("strings").get<pj::object>())
    {
        std::string key = p.first; // Specifically do not use ToStdWString here
                                // since we do not want to find Unicode keys
        std::wstring val = Utils::toStdWString(p.second.get<std::string>());

        l.translations.insert({ key, val });
    }

    translator->m_languages.insert({ l.code, l });

    return TRUE;
}

std::vector<Translator::Language> Translator::Languages()
{
    std::vector<Language> result;

    for (auto& p : m_languages)
    {
        result.push_back(p.second);
    }

    return result;
}

std::wstring Translator::Translate(std::string const& key)
{
    auto lang = m_languages.find(m_selectedLanguage);
    if (lang == m_languages.end()) { lang = m_languages.find(1033); }
    if (lang == m_languages.end()) { return Utils::toStdWString(key); }

    auto translation = lang->second.translations.find(key);

    if (translation == lang->second.translations.end())
    {
        return Utils::toStdWString(key);
    }

    return translation->second;
}

void Translator::SetLanguage(int langCode)
{
    m_selectedLanguage = langCode;
}
