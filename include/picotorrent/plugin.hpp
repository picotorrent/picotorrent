#pragma once

#include <memory>
#include <vector>
#include <windows.h>

#include <picotorrent/core/signals/signal.hpp>

namespace picotorrent
{
    struct plugin_config_window
    {
        virtual HWND handle() = 0;
        virtual void load() = 0;
        virtual void save() = 0;

        core::signals::signal_connector<void, void>& on_dirty()
        {
            return dirty;
        }

    protected:
        void emit_dirty() { dirty.emit(); }

    private:
        core::signals::signal<void, void> dirty;
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
