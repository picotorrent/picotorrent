#pragma once

namespace picotorrent { namespace extensibility { class plugin_host; } }

namespace PicoTorrent
{
    public interface class IPluginEngine
    {
        void LoadAll();
        void UnloadAll();
    };

    ref class PluginEngine : public IPluginEngine
    {
    public:
        PluginEngine(picotorrent::extensibility::plugin_host*);

        virtual void LoadAll();
        virtual void UnloadAll();

    private:
        IPluginHost^ _host;
        System::Collections::Generic::IList<PicoTorrent::IPlugin^>^ _plugins;
    };
}