#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

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
namespace client
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

        private:
            std::shared_ptr<picojson::object> cfg_;
        };

        struct session_part : public part
        {
            friend class configuration;

            int active_checking();
            int active_dht_limit();
            int active_downloads();
            int active_limit();
            int active_loaded_limit();
            int active_lsd_limit();
            int active_seeds();
            int active_tracker_limit();

            int download_rate_limit();
            void download_rate_limit(int limit);

            int stop_tracker_timeout();

            int upload_rate_limit();
            void upload_rate_limit(int limit);

        protected:
            using part::part;
        };

        struct websocket_part : public part
        {
            friend class configuration;

            std::string access_token();
            void access_token(const std::string &token);

            std::string certificate_file();
            std::string certificate_password();
            std::string cipher_list();

            bool enabled();
            void enabled(bool enable);

            int listen_port();
            void listen_port(int port);

        protected:
            using part::part;
        };

        configuration();
        ~configuration();

        static configuration &instance();

        std::shared_ptr<session_part> session();
        std::shared_ptr<websocket_part> websocket();

        close_action_t close_action();
        void set_close_action(close_action_t action);

        bool check_for_updates();

        int current_language_id();
        void set_current_language_id(int langId);
        
        std::string default_save_path();
        void set_default_save_path(const std::string &path);

        std::string ignored_update();
        void set_ignored_update(const std::string &version);

        std::vector<std::pair<std::string, int>> listen_interfaces();
        void set_listen_interfaces(const std::vector<std::pair<std::string, int>> &interfaces);

        bool prompt_for_remove_data();
        void set_prompt_for_remove_data(bool value);

        proxy_type_t proxy_type();
        void set_proxy_type(proxy_type_t type);

        std::string proxy_host();
        void set_proxy_host(const std::string &host);

        int proxy_port();
        void set_proxy_port(int port);

        std::string proxy_username();
        void set_proxy_username(const std::string &user);

        std::string proxy_password();
        void set_proxy_password(const std::string &pass);

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

        std::shared_ptr<core::session_configuration> session_configuration();

        start_position_t start_position();
        void set_start_position(start_position_t pos);

        std::string update_url();

        std::shared_ptr<placement> window_placement(const std::string &name);
        void set_window_placement(const std::string &name, const placement &wnd);

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
