#pragma once

namespace PicoTorrent
{
    ref class Configuration : public IConfiguration
    {
    public:
        Configuration();

        generic<typename T> virtual T Get(System::String^);
        generic<typename T> virtual void Set(System::String^, T);

    private:
        System::Web::Script::Serialization::JavaScriptSerializer^ _serializer;
    };
}
