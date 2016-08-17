#pragma once

#include <map>
#include <string>
#include <vector>

#include "StringUtils.hpp"

#define TR(id) Translator::GetInstance().Translate(id).c_str()
#define TRW(id) ToWideString(Translator::GetInstance().Translate(id)).c_str()

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

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
    Translator();
    ~Translator();

    std::wstring GetLanguagePath();

    HINSTANCE m_instance;
    picojson::object m_strings;
};
