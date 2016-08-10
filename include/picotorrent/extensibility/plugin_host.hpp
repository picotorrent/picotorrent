#pragma once

#include <windows.h>

#include <picotorrent/export.hpp>

namespace picotorrent
{
namespace core
{
    class session;
}
namespace extensibility
{
    class plugin_host
    {
    public:
        DLL_EXPORT plugin_host(core::session* sess, HWND hWndMainWindow);

        DLL_EXPORT HWND get_main_window();
        DLL_EXPORT core::session* get_session();

    private:
        core::session* sess_;
        HWND hWndMainWindow_;
    };
}
}
