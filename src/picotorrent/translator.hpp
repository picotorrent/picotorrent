#pragma once

#include <Windows.h>

#include <map>
#include <memory>
#include <vector>

#include <QString>

#define i18n(key) Translator::instance().translate(key)

namespace pt
{
    class Translator
    {
    public:
        struct Language
        {
            int code;
            QString name;
            std::map<QString, QString> translations;
        };

        static Translator& instance();

        Translator(Translator const&) = delete;
        void operator=(Translator const&) = delete;

        std::vector<Language> languages();
        void loadEmbedded(HINSTANCE hInstance);
        void loadFile(QString const& fileName);
        void setLanguage(int languageCode);
        QString translate(QString const& key);

    private:
        Translator();

        static BOOL CALLBACK enumLanguageFiles(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);

        int m_selectedLanguage;
        std::map<int, Language> m_languages;
    };
}
