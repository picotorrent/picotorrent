#pragma once

#include <memory>
#include <vector>
#include <windows.h>

namespace picotorrent
{
    struct plugin_config_window
    {
        virtual HWND handle() = 0;
        virtual void load() = 0;
        virtual void save() = 0;
    };

    class plugin
    {
    public:
        virtual std::string get_name() = 0;
        virtual std::string get_version() = 0;
        virtual std::shared_ptr<plugin_config_window> get_config_window() { return nullptr; }
        virtual void load() = 0;
        virtual void unload() = 0;
    };

    typedef std::shared_ptr<plugin> plugin_ptr;

    struct plugin_wrapper
    {
        std::vector<plugin_ptr> plugins;
    };
}
