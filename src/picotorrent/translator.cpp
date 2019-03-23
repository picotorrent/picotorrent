#include "translator.hpp"

#include <picotorrent/core/environment.hpp>

#include "loguru.hpp"
#include "picojson.hpp"

namespace pj = picojson;
using pt::Translator;

Translator::Translator()
{
}

Translator& Translator::instance()
{
    static Translator translator;
    return translator;
}

void Translator::loadEmbedded(HINSTANCE hInstance)
{
    EnumResourceNames(
        hInstance,
        TEXT("LANGFILE"),
        enumLanguageFiles,
        reinterpret_cast<LONG_PTR>(this));

    LOG_F(INFO, "Found %d embedded translation files", m_languages.size());
}

BOOL Translator::enumLanguageFiles(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
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
        LOG_F(ERROR, "Failed to parse language json: %s", err.data());
        return TRUE;
    }

    pj::object obj = v.get<pj::object>();

    int langId = static_cast<int>(obj.at("lang_id").get<int64_t>());
    std::string langName = obj.at("lang_name").get<std::string>();

    Language l;
    l.code = langId;
    l.name = QString::fromStdString(langName);

    for (auto& p : obj.at("strings").get<pj::object>())
    {
        QString val = QString::fromStdString(p.second.get<std::string>());
        l.translations.insert({ QString::fromStdString(p.first), val });
    }

    translator->m_languages.insert({ l.code, l });

    return TRUE;
}

std::vector<Translator::Language> Translator::languages()
{
    std::vector<Language> result;

    for (auto& p : m_languages)
    {
        result.push_back(p.second);
    }

    return result;
}

QString Translator::translate(QString const& key)
{
    auto lang = m_languages.find(m_selectedLanguage);
    if (lang == m_languages.end()) { lang = m_languages.find(1033); }
    if (lang == m_languages.end()) { return key; }

    auto translation = lang->second.translations.find(key);

    if (translation == lang->second.translations.end())
    {
        return key;
    }

    return translation->second;
}

void Translator::setLanguage(int langCode)
{
    m_selectedLanguage = langCode;
}
