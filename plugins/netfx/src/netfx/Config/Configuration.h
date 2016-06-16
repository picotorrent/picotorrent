#pragma once

namespace PicoTorrent
{
namespace Config
{
    ref class Configuration : public IConfiguration
    {
    public:
        virtual property System::String^ IgnoredUpdate { System::String^ get(); void set(System::String^); }
    };
}
}
