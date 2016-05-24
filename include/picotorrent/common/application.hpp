#pragma once

#include <memory>
#include <vector>

#include <picotorrent/export.hpp>

namespace picotorrent
{
namespace core
{
    class session;
    class torrent;
}
namespace common
{
namespace ws
{
    class websocket_server;
}

    class command_line;

    class application
    {
    public:
        DLL_EXPORT bool init();
        DLL_EXPORT int run(const command_line &cmd);

    protected:
        DLL_EXPORT std::shared_ptr<core::session> get_session();
        DLL_EXPORT std::shared_ptr<ws::websocket_server> get_websocket_server();

        // Initialization
        DLL_EXPORT virtual bool on_pre_init() { return true; };
        DLL_EXPORT virtual bool on_init() = 0;
        DLL_EXPORT virtual void on_post_init() { };

        // Running
        DLL_EXPORT virtual int on_run(const command_line &cmd) = 0;

        // Torrent handlers
        DLL_EXPORT void notifications_available();
        DLL_EXPORT void torrent_added(const std::shared_ptr<core::torrent> &torrent);
        DLL_EXPORT void torrent_finished(const std::shared_ptr<core::torrent> &torrent);
        DLL_EXPORT void torrent_removed(const std::shared_ptr<core::torrent> &torrent);
        DLL_EXPORT void torrent_updated(const std::vector<std::shared_ptr<core::torrent>> &torrents);

        DLL_EXPORT virtual void on_notifications_available() = 0;
        DLL_EXPORT virtual void on_torrent_added(const std::shared_ptr<core::torrent> &torrent) { }
        DLL_EXPORT virtual void on_torrent_finished(const std::shared_ptr<core::torrent> &torrent) { }
        DLL_EXPORT virtual void on_torrent_removed(const std::shared_ptr<core::torrent> &torrent) { }
        DLL_EXPORT virtual void on_torrent_updated(const std::vector<std::shared_ptr<core::torrent>> &torrents) { }

    private:
        std::shared_ptr<core::session> session_;
        std::vector<std::shared_ptr<core::torrent>> torrents_;
        std::shared_ptr<ws::websocket_server> ws_server_;
    };
}
}
