#pragma once

namespace picotorrent { namespace extensibility { class plugin_host; } }

namespace PicoTorrent
{
    public interface class IPluginEngine
    {
        System::Collections::Generic::IList<PicoTorrent::IPlugin^>^ GetAll();
    };

    ref class PluginEngine : public IPluginEngine
    {
    public:
        PluginEngine(picotorrent::extensibility::plugin_host*);
        virtual System::Collections::Generic::IList<PicoTorrent::IPlugin^>^ GetAll();

    private:
        picotorrent::extensibility::plugin_host* _ptr;
    };
}