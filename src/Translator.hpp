#pragma once

#include <string>

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
    std::string Translate(const std::string& key);
};
