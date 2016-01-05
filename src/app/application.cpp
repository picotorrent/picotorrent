#include <picotorrent/app/application.hpp>

#include <picotorrent/app/command_line.hpp>
#include <picotorrent/app/message_loop.hpp>
#include <picotorrent/app/controllers/add_torrent_controller.hpp>
#include <picotorrent/app/controllers/application_close_controller.hpp>
#include <picotorrent/app/controllers/application_update_controller.hpp>
#include <picotorrent/app/controllers/notifyicon_context_menu_controller.hpp>
#include <picotorrent/app/controllers/remove_torrent_controller.hpp>
#include <picotorrent/app/controllers/torrent_context_menu_controller.hpp>
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

    main_window_->on_close(std::bind(&application::on_close, this));
    main_window_->on_copydata(std::bind(&application::on_command_line_args, this, std::placeholders::_1));
    main_window_->on_notifyicon_context_menu(std::bind(&application::on_notifyicon_context_menu, this, std::placeholders::_1));
    main_window_->on_torrent_context_menu(std::bind(&application::on_torrent_context_menu, this, std::placeholders::_1, std::placeholders::_2));

    sess_->on_torrent_added(std::bind(&application::torrent_added, this, std::placeholders::_1));
    sess_->on_torrent_finished(std::bind(&application::torrent_finished, this, std::placeholders::_1));
    sess_->on_torrent_removed(std::bind(&application::torrent_removed, this, std::placeholders::_1));
    sess_->on_torrent_updated(std::bind(&application::torrent_updated, this, std::placeholders::_1));
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
    return result;
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

void application::on_remove_torrents_accelerator(bool remove_data)
{
    controllers::remove_torrent_controller remove_controller(
        main_window_,
        sess_,
        main_window_->get_selected_torrents());

    remove_controller.execute(remove_data);
}

void application::on_torrent_context_menu(const POINT &p, const std::vector<std::shared_ptr<core::torrent>> &torrents)
{
    controllers::torrent_context_menu_controller menu_controller(sess_, torrents, main_window_);
    menu_controller.execute(p);
}

void application::on_view_preferences()
{
    controllers::view_preferences_controller view_prefs(main_window_);
    view_prefs.execute();
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

void application::torrent_finished(const std::shared_ptr<core::torrent> &torrent)
{
    main_window_->post_message(WM_TORRENT_FINISHED, NULL, (LPARAM)&torrent);
}

void application::torrent_removed(const std::shared_ptr<core::torrent> &torrent)
{
    main_window_->send_message(WM_TORRENT_REMOVED, NULL, (LPARAM)&torrent);
}

void application::torrent_updated(const std::shared_ptr<core::torrent> &torrent)
{
    main_window_->post_message(WM_TORRENT_UPDATED, NULL, (LPARAM)&torrent);
}
