#include <picotorrent/app/controllers/notifyicon_context_menu_controller.hpp>

#include <picotorrent/app/controllers/add_torrent_controller.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/notifyicon_context_menu.hpp>
#include <picotorrent/ui/resources.hpp>

using picotorrent::app::controllers::notifyicon_context_menu_controller;
using picotorrent::core::session;
using picotorrent::ui::main_window;
using picotorrent::ui::notifyicon_context_menu;

notifyicon_context_menu_controller::notifyicon_context_menu_controller(
    const std::shared_ptr<session> &sess,
    const std::shared_ptr<main_window> &wnd)
    : sess_(sess),
    wnd_(wnd)
{
}

void notifyicon_context_menu_controller::execute(const POINT &p)
{
    // The WinApi tells us to do this, otherwise the menu will not close
    // correctly. Find out if there's another way - we don't want to show
    // the app when you right click the menu.
    SetForegroundWindow(wnd_->handle());

    notifyicon_context_menu menu;
    
    switch (menu.show(wnd_->handle(), p))
    {
    case ID_NOTIFYICON_ADD:
    {
        controllers::add_torrent_controller add(sess_, wnd_);
        add.execute();
        break;
    }
    case ID_NOTIFYICON_EXIT:
        wnd_->exit();
        break;
    }
}
