#pragma once

#include <map>
#include <string>
#include <vector>

#include "StringUtils.hpp"

#define TR(id) Translator::GetInstance().Translate(id).c_str()
#define TRW(id) ToWideString(Translator::GetInstance().Translate(id)).c_str()

class Translator
{
public:
    struct Language
    {
        std::string name;
        int code;
    };

    static Translator& GetInstance();

    std::vector<Language> GetAvailableLanguages();
    std::string Translate(const std::string& key);

private:
    static BOOL CALLBACK LoadTranslations(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);

    Translator();
    ~Translator();

    int m_currentLanguage;
    std::vector<Language> m_languages;
    std::map<int, std::map<std::string, std::string>> m_strings;
};
