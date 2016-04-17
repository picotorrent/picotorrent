#include <picotorrent/client/application.hpp>

#include <picotorrent/client/command_line.hpp>
#include <picotorrent/client/message_loop.hpp>
#include <picotorrent/client/controllers/add_magnet_link_controller.hpp>
#include <picotorrent/client/controllers/add_torrent_controller.hpp>
#include <picotorrent/client/controllers/application_close_controller.hpp>
#include <picotorrent/client/controllers/application_update_controller.hpp>
#include <picotorrent/client/controllers/notifyicon_context_menu_controller.hpp>
#include <picotorrent/client/controllers/remove_torrent_controller.hpp>
#include <picotorrent/client/controllers/torrent_context_menu_controller.hpp>
#include <picotorrent/client/controllers/torrent_details_controller.hpp>
#include <picotorrent/client/controllers/unhandled_exception_controller.hpp>
#include <picotorrent/client/controllers/view_preferences_controller.hpp>
#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/logging/log.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ws/websocket_server.hpp>

#include <picotorrent/core/session.hpp>
#include <picotorrent/core/session_configuration.hpp>

#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>
#include <shellapi.h>
#include <string.h>

namespace controllers = picotorrent::client::controllers;
namespace core = picotorrent::core;
namespace ui = picotorrent::client::ui;
using picotorrent::client::application;
using picotorrent::client::command_line;
using picotorrent::client::configuration;
using picotorrent::client::logging::log;
using picotorrent::client::ws::websocket_server;

application::application()
    : mtx_(NULL),
    accelerators_(LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PICO_ACCELERATORS)))
{
    log::instance().set_unhandled_exception_callback(std::bind(&application::on_unhandled_exception, this, std::placeholders::_1));
}

application::~application()
{
    if (mtx_ != NULL)
    {
        CloseHandle(mtx_);
        mtx_ = NULL;
    }
}

void application::activate_other_instance(const std::wstring &args)
{
    HWND otherWindow = FindWindow(L"PicoTorrent/MainWindow", NULL);

    COPYDATASTRUCT cds;

    if (!args.empty())
    {
        cds.cbData = (DWORD)(sizeof(wchar_t) * (args.size() + 1));
        cds.dwData = 1;
        cds.lpData = (PVOID)&args[0];
    }

    // Activate other window
    SetForegroundWindow(otherWindow);
    ShowWindow(otherWindow, SW_RESTORE);

    SendMessage(otherWindow, WM_COPYDATA, NULL, (LPARAM)&cds);
}

bool application::init()
{
    // Set our process to be DPI aware so we look good everywhere.
    if (!SetProcessDPIAware())
    {
        return false;
    }

    sess_ = std::make_shared<core::session>(configuration::instance().session_configuration());
    main_window_ = std::make_shared<ui::main_window>(sess_);
    ws_server_ = std::make_shared<websocket_server>(sess_);

    main_window_->on_command(ID_FILE_ADD_TORRENT, std::bind(&application::on_file_add_torrent, this));
    main_window_->on_command(ID_FILE_ADD_MAGNET_LINK, std::bind(&application::on_file_add_magnet_link, this));
    main_window_->on_command(IDA_REMOVE_TORRENTS, std::bind(&application::on_remove_torrents_accelerator, this, false));
    main_window_->on_command(IDA_REMOVE_TORRENTS_DATA, std::bind(&application::on_remove_torrents_accelerator, this, true));
    main_window_->on_command(IDA_SELECT_ALL, std::bind(&application::on_select_all_accelerator, this));
    main_window_->on_command(ID_VIEW_PREFERENCES, std::bind(&application::on_view_preferences, this));
    main_window_->on_command(ID_HELP_CHECK_FOR_UPDATE, std::bind(&application::on_check_for_update, this));

    main_window_->on_close(std::bind(&application::on_close, this));
    main_window_->on_copydata(std::bind(&application::on_command_line_args, this, std::placeholders::_1));
    main_window_->on_destroy().connect(std::bind(&application::on_destroy, this));
    main_window_->on_notifyicon_context_menu(std::bind(&application::on_notifyicon_context_menu, this, std::placeholders::_1));
    main_window_->on_session_alert_notify().connect(std::bind(&application::on_session_alert_notify, this));
    main_window_->on_torrent_activated(std::bind(&application::on_torrent_activated, this, std::placeholders::_1));
    main_window_->on_torrent_context_menu(std::bind(&application::on_torrent_context_menu, this, std::placeholders::_1, std::placeholders::_2));
    main_window_->on_torrents_dropped().connect(std::bind(&application::on_torrents_dropped, this, std::placeholders::_1));

    sess_->on_notifications_available().connect([this]()
    {
        PostMessage(main_window_->handle(), WM_USER + 1337, NULL, NULL);
    });

    sess_->on_torrent_added().connect(std::bind(&ui::main_window::torrent_added, main_window_, std::placeholders::_1));
    sess_->on_torrent_finished().connect(std::bind(&ui::main_window::torrent_finished, main_window_, std::placeholders::_1));
    sess_->on_torrent_removed().connect(std::bind(&ui::main_window::torrent_removed, main_window_, std::placeholders::_1));
    sess_->on_torrent_updated().connect(std::bind(&ui::main_window::torrent_updated, main_window_, std::placeholders::_1));

    INITCOMMONCONTROLSEX icex = { 0 };
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);

    if (!InitCommonControlsEx(&icex))
    {
        return false;
    }

    return true;
}

bool application::is_single_instance()
{
    mtx_ = CreateMutex(NULL, FALSE, L"PicoTorrent/1.0");
    DWORD err = GetLastError();

    if (err == ERROR_ALREADY_EXISTS)
    {
        LOG(info) << "PicoTorrent is already running";
        return false;
    }

    return true;
}

int application::run(const std::wstring &args)
{
    configuration &cfg = configuration::instance();
    UINT pos = SW_SHOWNORMAL;

    switch (cfg.start_position())
    {
    case configuration::start_position_t::hidden:
        pos = SW_HIDE;
        break;
    case configuration::start_position_t::minimized:
        pos = SW_SHOWMINIMIZED;
        break;
    }

    // Set window placement
    std::shared_ptr<configuration::placement> wp = cfg.window_placement("main");
    if (wp != nullptr)
    {
        WINDOWPLACEMENT winplace = { sizeof(WINDOWPLACEMENT) };
        winplace.flags = wp->flags;
        winplace.ptMaxPosition.x = wp->max_x;
        winplace.ptMaxPosition.y = wp->max_y;
        winplace.ptMinPosition.x = wp->min_x;
        winplace.ptMinPosition.y = wp->min_y;
        winplace.rcNormalPosition.bottom = wp->pos_bottom;
        winplace.rcNormalPosition.left = wp->pos_left;
        winplace.rcNormalPosition.right = wp->pos_right;
        winplace.rcNormalPosition.top = wp->pos_top;
        winplace.showCmd = pos;

        SetWindowPlacement(main_window_->handle(), &winplace);
    }
    else
    {
        ShowWindow(main_window_->handle(), pos);
    }

    ws_server_->start();
    sess_->load();

    if (!args.empty())
    {
        on_command_line_args(args);
    }

    updater_ = std::make_shared<controllers::application_update_controller>(main_window_);

    if (configuration::instance().check_for_updates())
    {
        updater_->execute();
    }

    int result = message_loop::run(main_window_->handle(), accelerators_);

    sess_->unload();
    ws_server_->stop();

    return result;
}

void application::wait_for_restart(const std::wstring &args)
{
    command_line cmd = command_line::parse(args);

    if (!cmd.restart())
    {
        return;
    }

    HANDLE hProc = OpenProcess(SYNCHRONIZE, FALSE, cmd.prev_process_id());

    if (hProc == NULL)
    {
        DWORD err = GetLastError();
        LOG(debug) << "Could not open process: " << err;
        return;
    }

    LOG(debug) << "Waiting for previous instance of PicoTorrent to shut down";
    DWORD res = WaitForSingleObject(hProc, 10000);
    CloseHandle(hProc);

    switch (res)
    {
    case WAIT_FAILED:
        LOG(debug) << "Could not wait for process: " << GetLastError();
        break;
    case WAIT_OBJECT_0:
        LOG(debug) << "Successfully waited for process";
        break;
    case WAIT_TIMEOUT:
        LOG(debug) << "Timeout when waiting for process";
        break;
    }
}

void application::on_check_for_update()
{
    updater_->execute(true);
}

bool application::on_close()
{
    controllers::application_close_controller close_controller(main_window_);
    return close_controller.execute();
}

void application::on_command_line_args(const std::wstring &args)
{
    command_line cmd = command_line::parse(args);

    controllers::add_torrent_controller add_controller(sess_, main_window_);
    add_controller.execute(cmd);
}

void application::on_destroy()
{
    WINDOWPLACEMENT wndplace = { sizeof(WINDOWPLACEMENT) };
    if (GetWindowPlacement(main_window_->handle(), &wndplace))
    {
        configuration::placement wp;
        wp.flags = wndplace.flags;
        wp.max_x = wndplace.ptMaxPosition.x;
        wp.max_y = wndplace.ptMaxPosition.y;
        wp.min_x = wndplace.ptMinPosition.x;
        wp.min_y = wndplace.ptMinPosition.y;
        wp.pos_bottom = wndplace.rcNormalPosition.bottom;
        wp.pos_left = wndplace.rcNormalPosition.left;
        wp.pos_right = wndplace.rcNormalPosition.right;
        wp.pos_top = wndplace.rcNormalPosition.top;
        wp.show = wndplace.showCmd;

        configuration::instance().set_window_placement("main", wp);
    }
}

void application::on_file_add_magnet_link()
{
    controllers::add_magnet_link_controller add_controller(sess_, main_window_);
    add_controller.execute();
}

void application::on_file_add_torrent()
{
    controllers::add_torrent_controller add_controller(sess_, main_window_);
    add_controller.execute();
}

void application::on_notifyicon_context_menu(const POINT &p)
{
    controllers::notifyicon_context_menu_controller notify_controller(sess_, main_window_);
    notify_controller.execute(p);
}

void application::on_select_all_accelerator()
{
    main_window_->select_all_torrents();
}

void application::on_session_alert_notify()
{
    sess_->notify();
}

void application::on_remove_torrents_accelerator(bool remove_data)
{
    controllers::remove_torrent_controller remove_controller(
        main_window_,
        sess_,
        main_window_->get_selected_torrents());

    remove_controller.execute(remove_data);
}

void application::on_torrent_activated(const std::shared_ptr<core::torrent> &torrent)
{
    controllers::torrent_details_controller details_controller(main_window_, torrent);
    details_controller.execute();
}

void application::on_torrent_context_menu(const POINT &p, const std::vector<std::shared_ptr<core::torrent>> &torrents)
{
    controllers::torrent_context_menu_controller menu_controller(sess_, torrents, main_window_);
    menu_controller.execute(p);
}

void application::on_torrents_dropped(const std::vector<std::string> &files)
{
    controllers::add_torrent_controller add_controller(sess_, main_window_);
    add_controller.execute(files);
}

void application::on_view_preferences()
{
    controllers::view_preferences_controller view_prefs(sess_, main_window_);
    view_prefs.execute();
}

void application::on_unhandled_exception(const std::string &stacktrace)
{
    controllers::unhandled_exception_controller exception_controller(main_window_, stacktrace);
    exception_controller.execute();
}
