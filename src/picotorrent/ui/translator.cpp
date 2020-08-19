#include "translator.hpp"

#include <loguru.hpp>
#include <nlohmann/json.hpp>

#include "../core/utils.hpp"

using json = nlohmann::json;
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

    std::string jsonData(buffer, static_cast<size_t>(size));
    json j;

    try
    {
        j = json::parse(jsonData);
    }
    catch (std::exception const& ex)
    {
        if (IS_INTRESOURCE(lpszName))
        {
            LOG_F(ERROR, "Failed to parse language json (%d): %s", reinterpret_cast<int>(lpszName), ex.what());
        }
        else
        {
            LOG_F(ERROR, "Failed to parse language json: %s", ex.what());
        }

        return TRUE;
    }

    int langId = j["lang_id"];
    std::string langName = j["lang_name"];

    Language l;
    l.code = langId;
    l.name = Utils::toStdWString(langName);

    for (auto& p : j["strings"].items())
    {
        std::string key = p.key(); // Specifically do not use ToStdWString here
                                   // since we do not want to find Unicode keys
        std::wstring val = Utils::toStdWString(p.value());

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

    std::sort(
        result.begin(),
        result.end(),
        [](Language const& lhs, Language const& rhs)
        {
            return lhs.name < rhs.name;
        });

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
