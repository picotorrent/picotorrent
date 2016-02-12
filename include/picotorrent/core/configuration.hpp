#pragma once

#include <map>
#include <memory>
#include <picotorrent/common.hpp>
#include <string>

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

namespace picotorrent
{
namespace core
{
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

        struct placement
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

        DLL_EXPORT configuration();
        DLL_EXPORT ~configuration();

        DLL_EXPORT static configuration &instance();

        DLL_EXPORT int alert_queue_size();

        DLL_EXPORT close_action_t close_action();
        DLL_EXPORT void set_close_action(close_action_t action);

        DLL_EXPORT bool check_for_updates();

        DLL_EXPORT int current_language_id();
        DLL_EXPORT void set_current_language_id(int langId);
        
        DLL_EXPORT std::wstring default_save_path();
        DLL_EXPORT void set_default_save_path(const std::wstring &path);

        DLL_EXPORT std::wstring ignored_update();
        DLL_EXPORT void set_ignored_update(const std::wstring &version);

        DLL_EXPORT std::wstring listen_address();
        DLL_EXPORT void set_listen_address(const std::wstring &address);
        
        DLL_EXPORT int listen_port();
        DLL_EXPORT void set_listen_port(int port);

        DLL_EXPORT bool prompt_for_remove_data();
        DLL_EXPORT void set_prompt_for_remove_data(bool value);

        DLL_EXPORT proxy_type_t proxy_type();
        DLL_EXPORT void set_proxy_type(proxy_type_t type);

        DLL_EXPORT std::wstring proxy_host();
        DLL_EXPORT void set_proxy_host(const std::wstring &host);

        DLL_EXPORT int proxy_port();
        DLL_EXPORT void set_proxy_port(int port);

        DLL_EXPORT std::wstring proxy_username();
        DLL_EXPORT void set_proxy_username(const std::wstring &user);

        DLL_EXPORT std::wstring proxy_password();
        DLL_EXPORT void set_proxy_password(const std::wstring &pass);

        DLL_EXPORT bool proxy_force();
        DLL_EXPORT void set_proxy_force(bool value);

        DLL_EXPORT bool proxy_hostnames();
        DLL_EXPORT void set_proxy_hostnames(bool value);

        DLL_EXPORT bool proxy_peers();
        DLL_EXPORT void set_proxy_peers(bool value);

        DLL_EXPORT bool proxy_trackers();
        DLL_EXPORT void set_proxy_trackers(bool value);

        DLL_EXPORT bool prompt_for_save_path();
        DLL_EXPORT void set_prompt_for_save_path(bool value);

        DLL_EXPORT int stop_tracker_timeout();
        DLL_EXPORT std::wstring update_url();

        DLL_EXPORT bool use_picotorrent_peer_id();
        DLL_EXPORT void set_use_picotorrent_peer_id(bool value);

        DLL_EXPORT std::shared_ptr<placement> window_placement(const std::string &name);
        DLL_EXPORT void set_window_placement(const std::string &name, const placement &wnd);

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
