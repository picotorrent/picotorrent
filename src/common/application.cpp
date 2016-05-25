#include <picotorrent/common/application.hpp>

#include <picotorrent/common/application_initializer.hpp>
#include <picotorrent/common/command_line.hpp>
#include <picotorrent/common/config/configuration.hpp>
#include <picotorrent/common/ws/websocket_server.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>

using picotorrent::common::application;
using picotorrent::common::application_initializer;
using picotorrent::common::command_line;
using picotorrent::common::config::configuration;
using picotorrent::common::ws::websocket_server;
using picotorrent::core::session;
using picotorrent::core::torrent;

application::application()
{
    mtx_ = CreateMutex(NULL, FALSE, L"PicoTorrent/1.0");
    is_single_instance_ = GetLastError() != ERROR_ALREADY_EXISTS;
}

application::~application()
{
    if (mtx_ != NULL)
    {
        CloseHandle(mtx_);
    }
}

bool application::init()
{
    if (!on_pre_init())
    {
        return false;
    }

    if (is_single_instance())
    {
        // Generate required configuration
        application_initializer app_init;
        app_init.create_application_paths();
        app_init.generate_websocket_access_token();
        app_init.generate_websocket_certificate();

        session_ = std::make_shared<core::session>(configuration::instance().session_configuration());
        session_->on_notifications_available().connect(std::bind(&application::notifications_available, this));
        session_->on_torrent_added().connect(std::bind(&application::torrent_added, this, std::placeholders::_1));
        session_->on_torrent_finished().connect(std::bind(&application::torrent_finished, this, std::placeholders::_1));
        session_->on_torrent_removed().connect(std::bind(&application::torrent_removed, this, std::placeholders::_1));
        session_->on_torrent_updated().connect(std::bind(&application::torrent_updated, this, std::placeholders::_1));

        ws_server_ = std::make_shared<ws::websocket_server>(session_);
    }

    if (!on_init())
    {
        return false;
    }

    on_post_init();
    return true;
}

int application::run(const command_line &cmd)
{
    // If we are not the single instance, we can run early and avoid
    // complicated if-conditions.
    if (!is_single_instance())
    {
        return on_run(cmd);
    }

    configuration &cfg = configuration::instance();

    if (cfg.websocket()->enabled())
    {
        ws_server_->start();
    }

    session_->load();

    int result = on_run(cmd);

    session_->unload();
    ws_server_->stop();

    return result;
}

std::shared_ptr<session> application::get_session()
{
    return session_;
}

std::shared_ptr<websocket_server> application::get_websocket_server()
{
    return ws_server_;
}

bool application::is_single_instance()
{
    return is_single_instance_;
}

void application::notifications_available()
{
    on_notifications_available();
}

void application::torrent_added(const std::shared_ptr<torrent> &torrent)
{
    torrents_.push_back(torrent);
    on_torrent_added(torrent);
}

void application::torrent_finished(const std::shared_ptr<torrent> &torrent)
{
    on_torrent_finished(torrent);
}

void application::torrent_removed(const std::shared_ptr<torrent> &torrent)
{
    auto &f = std::find(torrents_.begin(), torrents_.end(), torrent);
    if (f != torrents_.end()) { torrents_.erase(f); }

    on_torrent_removed(torrent);
}

void application::torrent_updated(const std::vector<std::shared_ptr<torrent>> &torrents)
{
    on_torrent_updated(torrents);
}
