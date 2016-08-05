#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <picotorrent/export.hpp>

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

namespace picotorrent
{
namespace core
{
    struct session_configuration;
}
namespace common
{
namespace config
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

        enum start_position_t
        {
            normal,
            minimized,
            hidden
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

        class part
        {
            friend class configuration;

        protected:
            part(const std::shared_ptr<picojson::object> &cfg);

            bool get_part_key_or_default(const char *part, const char* key, bool default_value);
            int get_part_key_or_default(const char *part, const char* key, int default_value);
            std::string get_part_key_or_default(const char *part, const char* key, const std::string &default_value);

            void set_part_key(const char *part, const char* key, bool value);
            void set_part_key(const char *part, const char* key, int value);
            void set_part_key(const char *part, const char* key, const std::string &value);

        protected:
            std::shared_ptr<picojson::object> cfg_;
        };

        struct session_part : public part
        {
            friend class configuration;

            DLL_EXPORT int active_checking();
            DLL_EXPORT int active_dht_limit();
            DLL_EXPORT int active_downloads();
            DLL_EXPORT int active_limit();
            DLL_EXPORT int active_loaded_limit();
            DLL_EXPORT int active_lsd_limit();
            DLL_EXPORT int active_seeds();
            DLL_EXPORT int active_tracker_limit();

            DLL_EXPORT int download_rate_limit();
            DLL_EXPORT void download_rate_limit(int limit);

            DLL_EXPORT int stop_tracker_timeout();

            DLL_EXPORT int upload_rate_limit();
            DLL_EXPORT void upload_rate_limit(int limit);

        protected:
            using part::part;
        };

        struct websocket_part : public part
        {
            friend class configuration;

            DLL_EXPORT std::string access_token();
            DLL_EXPORT void access_token(const std::string &token);

            DLL_EXPORT std::string certificate_file();
            DLL_EXPORT std::string certificate_password();
            DLL_EXPORT std::string cipher_list();

            DLL_EXPORT bool enabled();
            DLL_EXPORT void enabled(bool enable);

            DLL_EXPORT int listen_port();
            DLL_EXPORT void listen_port(int port);

        protected:
            using part::part;
        };

        struct ui_part : public part
        {
            friend class configuration;

            struct list_view_column_state
            {
                bool visible;
                int width;
            };

            DLL_EXPORT std::map<int, list_view_column_state> list_view_state(const std::string &id);
            DLL_EXPORT void list_view_state(const std::string &id, const std::map<int, list_view_column_state> &cols);

        protected:
            using part::part;
        };

        DLL_EXPORT configuration();
        DLL_EXPORT ~configuration();

        DLL_EXPORT static configuration &instance();

        DLL_EXPORT std::shared_ptr<session_part> session();
        DLL_EXPORT std::shared_ptr<websocket_part> websocket();
        DLL_EXPORT std::shared_ptr<ui_part> ui();

        DLL_EXPORT close_action_t close_action();
        DLL_EXPORT void set_close_action(close_action_t action);

        DLL_EXPORT bool check_for_updates();

        DLL_EXPORT int current_language_id();
        DLL_EXPORT void set_current_language_id(int langId);
        
        DLL_EXPORT std::string default_save_path();
        DLL_EXPORT void set_default_save_path(const std::string &path);

        DLL_EXPORT std::string ignored_update();
        DLL_EXPORT void set_ignored_update(const std::string &version);

        DLL_EXPORT std::vector<std::pair<std::string, int>> listen_interfaces();
        DLL_EXPORT void set_listen_interfaces(const std::vector<std::pair<std::string, int>> &interfaces);

        DLL_EXPORT bool prompt_for_remove_data();
        DLL_EXPORT void set_prompt_for_remove_data(bool value);

        DLL_EXPORT proxy_type_t proxy_type();
        DLL_EXPORT void set_proxy_type(proxy_type_t type);

        DLL_EXPORT std::string proxy_host();
        DLL_EXPORT void set_proxy_host(const std::string &host);

        DLL_EXPORT int proxy_port();
        DLL_EXPORT void set_proxy_port(int port);

        DLL_EXPORT std::string proxy_username();
        DLL_EXPORT void set_proxy_username(const std::string &user);

        DLL_EXPORT std::string proxy_password();
        DLL_EXPORT void set_proxy_password(const std::string &pass);

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

        DLL_EXPORT std::shared_ptr<core::session_configuration> session_configuration();

        DLL_EXPORT start_position_t start_position();
        DLL_EXPORT void set_start_position(start_position_t pos);

        DLL_EXPORT std::string update_url();

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
        void set(const char *name, int value);

        void load();
        void save();

        std::shared_ptr<picojson::object> value_;
    };
}
}
}
