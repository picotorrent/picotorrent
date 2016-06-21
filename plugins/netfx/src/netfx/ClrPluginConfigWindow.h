#pragma once

#include <picotorrent/plugin.hpp>
#include <vcclr.h>

ref class EventHandlerWrapper;

class ClrPluginConfigWindow : public picotorrent::plugin_config_window
{
public:
    ClrPluginConfigWindow(gcroot<PicoTorrent::UI::IConfigurationWindow^> instance);

    HWND handle();
    void load();
    void save();

    void raise_dirty();

private:
    gcroot<EventHandlerWrapper^> _eventWrapper;
    gcroot<PicoTorrent::UI::IConfigurationWindow^> _instance;
};

