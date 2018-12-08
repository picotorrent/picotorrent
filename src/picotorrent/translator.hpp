#pragma once

#include <map>
#include <memory>
#include <vector>

#include <QString>

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
            QString name;
            std::map<QString, QString> translations;
        };

        static std::shared_ptr<Translator> load(HINSTANCE hInstance, std::shared_ptr<Configuration> config);

        std::vector<Language> getAvailableLanguages();
        QString translate(QString key);

    private:
        Translator(std::map<int, Language> const& languages, int selectedLanguage);

        static BOOL CALLBACK loadTranslationResource(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);
        static bool loadLanguageFromJson(std::string const& json, Language& lang);

        int m_selectedLanguage;
        std::map<int, Language> m_languages;
    };
}
