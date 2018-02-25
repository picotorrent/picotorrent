#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <map>
#include <memory>
#include <vector>

#define i18n(t, s) t->Translate(##s)

namespace pt
{
    class Configuration;

    class Translator
    {
    public:
        struct Language
        {
            int code;
            wxString name;
            std::map<wxString, wxString> translations;
        };


        static std::shared_ptr<Translator> Load(HINSTANCE hInstance, std::shared_ptr<Configuration> config);

        std::vector<Language> GetAvailableLanguages();
        wxString Translate(wxString key);

    private:
        Translator(std::map<int, Language> const& languages, int selectedLanguage);

        static BOOL CALLBACK LoadTranslationResource(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);

        int m_selectedLanguage;
        std::map<int, Language> m_languages;
    };
}
