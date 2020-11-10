#pragma once

#include <filesystem>
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
            std::string locale;
            std::wstring name;
            std::map<std::string, std::wstring> translations;
        };

        static Translator& GetInstance();

        Translator(Translator const&) = delete;
        void operator=(Translator const&) = delete;

        std::string GetLocale();
        std::vector<Language> Languages();
        void LoadDatabase(std::filesystem::path const& filePath);
        void SetLocale(std::string const& localeName);
        std::wstring Translate(std::string const& key);

    private:
        Translator();

        static int LoadDatabaseCallback(void* user, int count, char** data, char** columns);

        std::string m_selectedLocale;
        std::map<std::string, Language> m_languages;
    };
}
}
