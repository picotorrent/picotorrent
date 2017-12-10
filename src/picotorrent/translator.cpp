#include "translator.hpp"

#include "picojson.hpp"

namespace pj = picojson;
using pt::Translator;

Translator::Translator(std::map<int, Language> const& languages)
    : m_languages(languages),
    m_selectedLanguage(1033)
{
}

wxString Translator::Translate(wxString key)
{
    auto lang = m_languages.find(m_selectedLanguage);
    if (lang == m_languages.end()) { lang = m_languages.find(1033); }

    auto translation = lang->second.translations.find(key);

    if (translation == lang->second.translations.end())
    {
        return key;
    }

    return translation->second;
}

std::shared_ptr<Translator> Translator::Load(HINSTANCE hInstance)
{
    // TODO: Very Win32 specific code, enumerating the embedded resources.
    // Should be split into a platform specific layer when making PicoTorrent
    // cross platform.

    std::map<int, Language> languages;

    EnumResourceNames(
        hInstance,
        TEXT("LANGFILE"),
        LoadTranslationResource,
        reinterpret_cast<LONG_PTR>(&languages));

    return std::shared_ptr<Translator>(new Translator(languages));
}

BOOL Translator::LoadTranslationResource(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
    std::map<int, Language>* languages = reinterpret_cast<std::map<int, Language>*>(lParam);

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

    Language l;
    l.code = langId;
    l.name = langName;

    for (auto& p : obj.at("strings").get<pj::object>())
    {
        l.translations.insert({ p.first, p.second.get<std::string>() });
    }

    languages->insert({ langId, l });

    return TRUE;
}
