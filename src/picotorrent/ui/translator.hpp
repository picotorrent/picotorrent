#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

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
            wxString name;
            std::map<wxString, wxString> translations;
        };

        static Translator& GetInstance();

        Translator(Translator const&) = delete;
        void operator=(Translator const&) = delete;

        std::vector<Language> Languages();
        void LoadEmbedded(HINSTANCE hInstance);
        void SetLanguage(int languageCode);
        wxString Translate(wxString const& key);

    private:
        Translator();

        static BOOL CALLBACK EnumLanguageFiles(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);

        int m_selectedLanguage;
        std::map<int, Language> m_languages;
    };
}
}
