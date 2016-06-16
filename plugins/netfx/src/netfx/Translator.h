#pragma once

namespace PicoTorrent
{
    ref class Translator : public ITranslator
    {
    public:
        virtual System::String^ Translate(System::String^ key);
    };
}
