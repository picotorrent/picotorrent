#pragma once

#include <map>
#include <memory>
#include <string>

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

namespace picotorrent
{
namespace config
{
    struct window_placement
    {
        uint32_t flags;
        long max_x;
        long max_y;
        long min_x;
        long min_y;
        long pos_bottom;
        long pos_left;
        long pos_right;
        long pos_top;
        uint32_t show;
    };

    class configuration
    {
    public:
        enum close_action_t
        {
            prompt,
            close,
            minimize
        };

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

        configuration();
        ~configuration();

        static configuration &instance();

        int alert_queue_size();

        close_action_t close_action();
        void set_close_action(close_action_t action);

        bool check_for_updates();

        int current_language_id();
        void set_current_language_id(int langId);
        
        std::wstring default_save_path();
        void set_default_save_path(const std::wstring &path);

        std::wstring ignored_update();
        void set_ignored_update(const std::wstring &version);

        std::wstring listen_address();
        void set_listen_address(const std::wstring &address);
        
        int listen_port();
        void set_listen_port(int port);

        bool prompt_for_remove_data();
        void set_prompt_for_remove_data(bool value);

        proxy_type_t proxy_type();
        void set_proxy_type(proxy_type_t type);

        std::wstring proxy_host();
        void set_proxy_host(const std::wstring &host);

        int proxy_port();
        void set_proxy_port(int port);

        std::wstring proxy_username();
        void set_proxy_username(const std::wstring &user);

        std::wstring proxy_password();
        void set_proxy_password(const std::wstring &pass);

        bool proxy_force();
        void set_proxy_force(bool value);

        bool proxy_hostnames();
        void set_proxy_hostnames(bool value);

        bool proxy_peers();
        void set_proxy_peers(bool value);

        bool proxy_trackers();
        void set_proxy_trackers(bool value);

        bool prompt_for_save_path();
        void set_prompt_for_save_path(bool value);

        int stop_tracker_timeout();
        std::wstring update_url();

        bool use_picotorrent_peer_id();
        void set_use_picotorrent_peer_id(bool value);

        std::shared_ptr<window_placement> window_placement(const std::string &name);
        void set_window_placement(const std::string &name, const config::window_placement &wnd);

    private:
        template<typename T>
        T get_or_default(const char *name, T defaultValue);

        template<typename T>
        void set(const char *name, T value);

        template<>
        int get_or_default(const char *name, int defaultValue);

        template<>
        std::wstring get_or_default(const char *name, std::wstring defaultValue);

        template<>
        void set(const char *name, int value);

        template<>
        void set(const char *name, std::wstring value);

        void load();
        void save();

        std::unique_ptr<picojson::object> value_;
    };
}
}
