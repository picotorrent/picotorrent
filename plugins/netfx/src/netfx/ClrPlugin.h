#pragma once

#include <picotorrent/plugin.hpp>
#include <vcclr.h>

class ClrPlugin : public picotorrent::plugin
{
public:
    ClrPlugin(gcroot<PicoTorrent::IPlugin^> instance);

    std::string get_name();
    std::string get_version();
    std::shared_ptr<picotorrent::plugin_config_window> get_config_window();
    void load();
    void unload();

private:
    std::shared_ptr<picotorrent::plugin_config_window> _cfg;
    gcroot<PicoTorrent::IPlugin^> _instance;
};

