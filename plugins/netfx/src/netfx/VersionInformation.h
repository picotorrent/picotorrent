#pragma once

namespace PicoTorrent
{
    ref class VersionInformation : public IVersionInformation
    {
    public:
        virtual property System::String^ Branch { System::String^ get(); }
        virtual property System::String^ Commitish { System::String^ get(); }
        virtual property System::String^ CurrentVersion { System::String^ get(); }
    };
}
