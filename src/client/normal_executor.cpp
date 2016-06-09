#include <picotorrent/client/normal_executor.hpp>

#include <functional>

#include <picotorrent/plugin.hpp>
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
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/common/command_line.hpp>
#include <picotorrent/common/config/configuration.hpp>
#include <picotorrent/common/ws/websocket_server.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>

namespace controllers = picotorrent::client::controllers;
using picotorrent::plugin;
using picotorrent::client::message_loop;
using picotorrent::client::normal_executor;
using picotorrent::common::command_line;
using picotorrent::common::config::configuration;
using picotorrent::common::ws::websocket_server;
using picotorrent::core::session;
using picotorrent::core::torrent;

normal_executor::normal_executor(
    const std::shared_ptr<session> &session,
    const std::shared_ptr<websocket_server> &ws_server)

    : accelerators_(LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PICO_ACCELERATORS))),
    session_(session),
    ws_server_(ws_server)
{
    main_window_ = std::make_shared<ui::main_window>(session_);
    main_window_->on_command(ID_FILE_ADD_TORRENT, std::bind(&normal_executor::on_file_add_torrent, this));
    main_window_->on_command(ID_FILE_ADD_MAGNET_LINK, std::bind(&normal_executor::on_file_add_magnet_link, this));
    main_window_->on_command(IDA_REMOVE_TORRENTS, std::bind(&normal_executor::on_remove_torrents_accelerator, this, false));
    main_window_->on_command(IDA_REMOVE_TORRENTS_DATA, std::bind(&normal_executor::on_remove_torrents_accelerator, this, true));
    main_window_->on_command(IDA_SELECT_ALL, std::bind(&normal_executor::on_select_all_accelerator, this));
    main_window_->on_command(ID_VIEW_PREFERENCES, std::bind(&normal_executor::on_view_preferences, this));
    main_window_->on_command(ID_HELP_CHECK_FOR_UPDATE, std::bind(&normal_executor::on_check_for_update, this));

    main_window_->on_close(std::bind(&normal_executor::on_close, this));
    main_window_->on_copydata(std::bind(&normal_executor::on_copydata, this, std::placeholders::_1));
    main_window_->on_destroy().connect(std::bind(&normal_executor::on_destroy, this));
    main_window_->on_notifyicon_context_menu(std::bind(&normal_executor::on_notifyicon_context_menu, this, std::placeholders::_1));
    main_window_->on_session_alert_notify().connect(std::bind(&normal_executor::on_session_alert_notify, this));
    main_window_->on_torrent_activated(std::bind(&normal_executor::on_torrent_activated, this, std::placeholders::_1));
    main_window_->on_torrent_context_menu(std::bind(&normal_executor::on_torrent_context_menu, this, std::placeholders::_1, std::placeholders::_2));
    main_window_->on_torrents_dropped().connect(std::bind(&normal_executor::on_torrents_dropped, this, std::placeholders::_1));
}

int normal_executor::run(const command_line &cmd)
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

    if (!cmd.files().empty() || !cmd.magnet_links().empty())
    {
        on_command_line_args(cmd);
    }

    updater_ = std::make_shared<controllers::application_update_controller>(main_window_);

    if (cfg.check_for_updates())
    {
        updater_->execute();
    }

    HMODULE hMod = LoadLibrary(L"PicoTorrentPlugin.dll");
    FARPROC proc = GetProcAddress(hMod, "create_plugin_host");

    typedef plugin*(*CREATE_PLUGIN_FUNC)(session*);
    CREATE_PLUGIN_FUNC cpf = (CREATE_PLUGIN_FUNC)proc;

    plugin* p = cpf(session_.get());
    p->load();

    return message_loop::run(main_window_->handle(), accelerators_);
}

void normal_executor::notification_available()
{
    PostMessage(main_window_->handle(), WM_USER + 1337, NULL, NULL);
}

void normal_executor::torrent_added(const std::shared_ptr<torrent> &t)
{
    main_window_->torrent_added(t);
}

void normal_executor::torrent_finished(const std::shared_ptr<torrent> &t)
{
    main_window_->torrent_finished(t);
}

void normal_executor::torrent_removed(const std::shared_ptr<torrent> &t)
{
    main_window_->torrent_removed(t);
}

void normal_executor::torrent_updated(const std::vector<std::shared_ptr<torrent>> &t)
{
    main_window_->torrent_updated(t);
}

void normal_executor::on_check_for_update()
{
    updater_->execute(true);
}

bool normal_executor::on_close()
{
    controllers::application_close_controller close_controller(main_window_);
    return close_controller.execute();
}

void normal_executor::on_command_line_args(const command_line &cmd)
{
    controllers::add_torrent_controller add_controller(session_, main_window_);
    add_controller.execute(cmd);
}

void normal_executor::on_copydata(const std::wstring &args)
{
    command_line cmd = command_line::parse(args);
    on_command_line_args(cmd);
}

void normal_executor::on_destroy()
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

void normal_executor::on_file_add_magnet_link()
{
    controllers::add_magnet_link_controller add_controller(session_, main_window_);
    add_controller.execute();
}

void normal_executor::on_file_add_torrent()
{
    controllers::add_torrent_controller add_controller(session_, main_window_);
    add_controller.execute();
}

void normal_executor::on_notifyicon_context_menu(const POINT &p)
{
    controllers::notifyicon_context_menu_controller notify_controller(session_, main_window_);
    notify_controller.execute(p);
}

void normal_executor::on_select_all_accelerator()
{
    main_window_->select_all_torrents();
}

void normal_executor::on_session_alert_notify()
{
    session_->notify();
}

void normal_executor::on_remove_torrents_accelerator(bool remove_data)
{
    controllers::remove_torrent_controller remove_controller(
        main_window_,
        session_,
        main_window_->get_selected_torrents());

    remove_controller.execute(remove_data);
}

void normal_executor::on_torrent_activated(const std::shared_ptr<torrent> &torrent)
{
    controllers::torrent_details_controller details_controller(main_window_, torrent);
    details_controller.execute();
}

void normal_executor::on_torrent_context_menu(const POINT &p, const std::vector<std::shared_ptr<torrent>> &torrents)
{
    controllers::torrent_context_menu_controller menu_controller(session_, torrents, main_window_);
    menu_controller.execute(p);
}

void normal_executor::on_torrents_dropped(const std::vector<std::string> &files)
{
    controllers::add_torrent_controller add_controller(session_, main_window_);
    add_controller.execute(files);
}

void normal_executor::on_view_preferences()
{
    controllers::view_preferences_controller view_prefs(
        session_,
        main_window_,
        ws_server_);

    view_prefs.execute();
}
