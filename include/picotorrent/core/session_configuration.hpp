#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace picotorrent
{
namespace core
{
    struct session_configuration
    {
        enum proxy_type_t
        {
            none,
            socks4,
            socks5,
            socks5_pw,
            http,
            http_pw,
            i2p
        };

        session_configuration()
            : alert_queue_size(500),
            enable_dht(true),
            stop_tracker_timeout(1),
            session_log_stream(new std::ostringstream())
        {
        }

        // Limits
        int active_checking;
        int active_dht_limit;
        int active_downloads;
        int active_limit;
        int active_loaded_limit;
        int active_lsd_limit;
        int active_seeds;
        int active_tracker_limit;

        int alert_queue_size;
        std::string default_save_path;
        int download_rate_limit;
        bool enable_dht;
        std::vector<std::pair<std::string, int>> listen_interfaces;
        std::unique_ptr<std::ostream> session_log_stream;
        std::string session_state_file;
        int stop_tracker_timeout;
        std::string temporary_directory;
        std::string torrents_directory;
        int upload_rate_limit;

        // Proxy settings
        proxy_type_t proxy_type;
        std::string proxy_host;
        int proxy_port;
        bool proxy_force;
        bool proxy_hostnames;
        bool proxy_peers;
        bool proxy_trackers;
    };
}
}
