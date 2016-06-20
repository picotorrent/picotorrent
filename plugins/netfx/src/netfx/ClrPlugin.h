#pragma once

#include <picotorrent/plugin.hpp>
#include <vcclr.h>

class ClrPlugin : public picotorrent::plugin
{
public:
    ClrPlugin(gcroot<PicoTorrent::IPlugin^> instance);

    std::string get_name();
    std::string get_version();
    HWND get_window();
    void load();
    void unload();

private:
    gcroot<PicoTorrent::IPlugin^> _instance;
};

