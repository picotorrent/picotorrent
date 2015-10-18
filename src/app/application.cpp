#include <picotorrent/app/application.hpp>

#include <picotorrent/app/message_loop.hpp>
#include <picotorrent/app/controllers/add_torrent_controller.hpp>
#include <picotorrent/app/controllers/unhandled_exception_controller.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/logging/log.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/resources.hpp>
#include <windows.h>
#include <commctrl.h>

namespace core = picotorrent::core;
namespace ui = picotorrent::ui;
using picotorrent::app::application;
using picotorrent::logging::log;

application::application()
    : main_window_(std::make_shared<ui::main_window>()),
    sess_(std::make_shared<core::session>())
{
    log::instance().set_unhandled_exception_callback(std::bind(&application::on_unhandled_exception, this, std::placeholders::_1));

    main_window_->on_command(ID_FILE_ADDTORRENT, std::bind(&application::on_file_add_torrent, this));

    sess_->on_torrent_added(std::bind(&application::torrent_added, this, std::placeholders::_1));
    sess_->on_torrent_updated(std::bind(&application::torrent_updated, this, std::placeholders::_1));
}

application::~application()
{
}

bool application::init()
{
    // Set our process to be DPI aware so we look good everywhere.
    if (!SetProcessDPIAware())
    {
        return false;
    }

    INITCOMMONCONTROLSEX icex = { 0 };
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);

    if (!InitCommonControlsEx(&icex))
    {
        return false;
    }

    return true;
}

int application::run()
{
    main_window_->create();
    sess_->load();

    int result = message_loop::run();

    sess_->unload();
    return result;
}

void application::on_file_add_torrent()
{
    controllers::add_torrent_controller add_controller(sess_, main_window_);
    add_controller.execute();
}

void application::on_unhandled_exception(const std::string &stacktrace)
{
    controllers::unhandled_exception_controller exception_controller(main_window_, stacktrace);
    exception_controller.execute();
}

void application::torrent_added(const std::shared_ptr<core::torrent> &torrent)
{
    main_window_->post_message(WM_TORRENT_ADDED, NULL, (LPARAM)&torrent);
}

void application::torrent_updated(const std::shared_ptr<core::torrent> &torrent)
{
    main_window_->post_message(WM_TORRENT_UPDATED, NULL, (LPARAM)&torrent);
}
