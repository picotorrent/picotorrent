#pragma once

#include <Windows.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#define i18n(key) pt::UI::Translator::GetInstance().Translate(key)

namespace pt
{
namespace UI
{
    class Translator
    {
    public:
        struct Language
        {
            int code;
            std::wstring name;
            std::map<std::string, std::wstring> translations;
        };

        static Translator& GetInstance();

        Translator(Translator const&) = delete;
        void operator=(Translator const&) = delete;

        std::vector<Language> Languages();
        void LoadEmbedded(HINSTANCE hInstance);
        void SetLanguage(int languageCode);
        std::wstring Translate(std::string const& key);

    private:
        Translator();

        static BOOL CALLBACK EnumLanguageFiles(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);

        int m_selectedLanguage;
        std::map<int, Language> m_languages;
    };
}
}
