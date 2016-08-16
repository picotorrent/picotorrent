#include "Translator.hpp"

Translator& Translator::GetInstance()
{
    static Translator instance;
    return instance;
}

std::string Translator::Translate(const std::string& key)
{
    return key;
}
