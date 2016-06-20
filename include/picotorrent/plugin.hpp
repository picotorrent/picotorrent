#pragma once

#include <memory>
#include <vector>
#include <windows.h>

namespace picotorrent
{
    class plugin
    {
    public:
        virtual std::string get_name() = 0;
        virtual std::string get_version() = 0;
        virtual HWND get_window() { return NULL; }
        virtual void load() = 0;
        virtual void unload() = 0;
    };

    typedef std::shared_ptr<plugin> plugin_ptr;

    struct plugin_wrapper
    {
        std::vector<plugin_ptr> plugins;
    };
}
