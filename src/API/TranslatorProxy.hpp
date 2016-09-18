#pragma once

#include <picotorrent/api.hpp>

namespace API
{
    class TranslatorProxy : public ITranslator
    {
    public:
        std::wstring Translate(std::string const& key);
    };
}
