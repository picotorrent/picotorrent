#include <picotorrent/extensibility/plugin_host.hpp>

#include <picotorrent/core/session.hpp>

using picotorrent::core::session;
using picotorrent::extensibility::plugin_host;

plugin_host::plugin_host(session* sess, HWND hWndMainWindow)
    : sess_(sess),
    hWndMainWindow_(hWndMainWindow)
{
}

HWND plugin_host::get_main_window()
{
    return hWndMainWindow_;
}

session* plugin_host::get_session()
{
    return sess_;
}
