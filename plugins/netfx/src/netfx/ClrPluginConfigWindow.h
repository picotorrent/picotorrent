#pragma once

#include <picotorrent/plugin.hpp>
#include <vcclr.h>

class ClrPluginConfigWindow : public picotorrent::plugin_config_window
{
public:
    ClrPluginConfigWindow(gcroot<PicoTorrent::UI::IConfigurationWindow^> instance);

    HWND handle();
    void load();
    void save();

private:
    gcroot<PicoTorrent::UI::IConfigurationWindow^> _instance;
};

