#include <picotorrent/app/application.hpp>

#include <picotorrent/app/command_line.hpp>
#include <picotorrent/app/message_loop.hpp>
#include <picotorrent/app/controllers/add_torrent_controller.hpp>
#include <picotorrent/app/controllers/application_close_controller.hpp>
#include <picotorrent/app/controllers/application_update_controller.hpp>
#include <picotorrent/app/controllers/notifyicon_context_menu_controller.hpp>
#include <picotorrent/app/controllers/remove_torrent_controller.hpp>
#include <picotorrent/app/controllers/torrent_context_menu_controller.hpp>
#include <picotorrent/app/controllers/torrent_details_controller.hpp>
#include <picotorrent/app/controllers/unhandled_exception_controller.hpp>
#include <picotorrent/app/controllers/view_preferences_controller.hpp>
#include <picotorrent/config/configuration.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/logging/log.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/resources.hpp>
#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>
#include <shellapi.h>
#include <string.h>

namespace core = picotorrent::core;
namespace fs = picotorrent::filesystem;
namespace ui = picotorrent::ui;
using picotorrent::app::application;
using picotorrent::app::command_line;
using picotorrent::config::configuration;
using picotorrent::logging::log;

application::application()
    : mtx_(NULL),
    main_window_(std::make_shared<ui::main_window>()),
    sess_(std::make_shared<core::session>()),
    accelerators_(LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PICO_ACCELERATORS)))
{
    log::instance().set_unhandled_exception_callback(std::bind(&application::on_unhandled_exception, this, std::placeholders::_1));

    main_window_->on_command(ID_FILE_ADDTORRENT, std::bind(&application::on_file_add_torrent, this));
    main_window_->on_command(IDA_REMOVE_TORRENTS, std::bind(&application::on_remove_torrents_accelerator, this, false));
    main_window_->on_command(IDA_REMOVE_TORRENTS_DATA, std::bind(&application::on_remove_torrents_accelerator, this, true));
    main_window_->on_command(IDA_SELECT_ALL, std::bind(&application::on_select_all_accelerator, this));
    main_window_->on_command(ID_VIEW_PREFERENCES, std::bind(&application::on_view_preferences, this));
    main_window_->on_command(ID_HELP_CHECK_FOR_UPDATE, std::bind(&application::on_check_for_update, this));

    main_window_->on_close(std::bind(&application::on_close, this));
    main_window_->on_copydata(std::bind(&application::on_command_line_args, this, std::placeholders::_1));
    main_window_->on_notifyicon_context_menu(std::bind(&application::on_notifyicon_context_menu, this, std::placeholders::_1));
    main_window_->on_session_alert_notify().connect(std::bind(&application::on_session_alert_notify, this));
    main_window_->on_torrent_activated(std::bind(&application::on_torrent_activated, this, std::placeholders::_1));
    main_window_->on_torrent_context_menu(std::bind(&application::on_torrent_context_menu, this, std::placeholders::_1, std::placeholders::_2));

    sess_->on_torrent_added().connect(std::bind(&ui::main_window::torrent_added, main_window_, std::placeholders::_1));
    sess_->on_torrent_finished().connect(std::bind(&ui::main_window::torrent_finished, main_window_, std::placeholders::_1));
    sess_->on_torrent_removed().connect(std::bind(&ui::main_window::torrent_removed, main_window_, std::placeholders::_1));
    sess_->on_torrent_updated().connect(std::bind(&ui::main_window::torrent_updated, main_window_, std::placeholders::_1));
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
    cds.cbData = (DWORD)(sizeof(wchar_t) * (args.size() + 1));
    cds.dwData = 1;
    cds.lpData = (PVOID)&args[0];

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
    main_window_->create();
    sess_->load(main_window_->handle());

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
