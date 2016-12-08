#include "TranslatorProxy.hpp"

#include "../Translator.hpp"

using API::TranslatorProxy;

std::wstring TranslatorProxy::Translate(std::string const& key)
{
    return TRW(key);
}
