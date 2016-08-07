#pragma once

namespace PicoTorrent
{
    ref class Environment : public IEnvironment
    {
    public:
        virtual property System::String^ ApplicationPath { System::String^ get(); }
        virtual property System::String^ DataPath { System::String^ get(); }
        virtual property bool IsInstalled { bool get(); }
    };
}
