#pragma once

#include <windows.h>

namespace picotorrent { namespace core { class session; } }

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
        PluginEngine(picotorrent::core::session*, HWND hWnd);

        virtual void LoadAll();
        virtual void UnloadAll();

    private:
        IPluginHost^ _host;
        System::Collections::Generic::IList<PicoTorrent::IPlugin^>^ _plugins;
    };
}