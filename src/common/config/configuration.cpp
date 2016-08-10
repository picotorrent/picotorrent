#include <picotorrent/common/config/configuration.hpp>

#include <picojson.hpp>
#include <picotorrent/common/environment.hpp>
#include <picotorrent/core/pal.hpp>
#include <picotorrent/core/session_configuration.hpp>

#include <fstream>
#include <sstream>

#include <windows.h>

namespace pj = picojson;
using picotorrent::common::config::configuration;
using picotorrent::core::pal;
using picotorrent::core::session_configuration;

configuration::configuration()
{
    load();
}

configuration::~configuration()
{
    save();
}

configuration& configuration::instance()
{
    static configuration instance;
    return instance;
}

pj::value configuration::get(const std::string &key)
{
    if (value_->find(key) == value_->end()) { return pj::value(); }
    return value_->at(key);
}

void configuration::set(const std::string &key, const pj::value &val)
{
    (*value_)[key] = val;
}

std::shared_ptr<configuration::plugins_part> configuration::plugins()
{
    // Not using make_shared since that function is not a friend of
    // the protected ctor.
    return std::shared_ptr<plugins_part>(new plugins_part(value_));
}

std::shared_ptr<configuration::session_part> configuration::session()
{
    // Not using make_shared since that function is not a friend of
    // the protected ctor.
    return std::shared_ptr<session_part>(new session_part(value_));
}

std::shared_ptr<configuration::websocket_part> configuration::websocket()
{
    // Not using make_shared since that function is not a friend of
    // the protected ctor.
    return std::shared_ptr<websocket_part>(new websocket_part(value_));
}

std::shared_ptr<configuration::ui_part> configuration::ui()
{
    // Not using make_shared since that function is not a friend of
    // the protected ctor.
    return std::shared_ptr<ui_part>(new ui_part(value_));
}

configuration::close_action_t configuration::close_action()
{
    return (configuration::close_action_t)get_or_default("close_action", (int64_t)configuration::close_action_t::prompt);
}

void configuration::set_close_action(configuration::close_action_t action)
{
    set("close_action", (int)action);
}

bool configuration::check_for_updates()
{
    return get_or_default("check_for_updates", true);
}

int configuration::current_language_id()
{
    return get_or_default("language_id", (int)GetUserDefaultLangID());
}

void configuration::set_current_language_id(int langId)
{
    set("language_id", langId);
}

std::string configuration::default_save_path()
{
    special_folder default_folder = environment::is_running_as_windows_service()
        ? special_folder::public_downloads
        : special_folder::user_downloads;

    std::string default_path = environment::get_special_folder(default_folder);
    return get_or_default("default_save_path", default_path);
}

void configuration::set_default_save_path(const std::string &path)
{
    set("default_save_path", path);
}

std::string configuration::ignored_update()
{
    return get_or_default<std::string>("ignored_update", "");
}

void configuration::set_ignored_update(const std::string &version)
{
    set("ignored_update", version);
}

std::vector<std::pair<std::string, int>> configuration::listen_interfaces()
{
    std::vector<std::pair<std::string, int>> defaultInterfaces = {
        { "0.0.0.0", 6881 },
        { "[::]", 6881 }
    };

    if (value_->find("listen_interfaces") == value_->end())
    {
        return defaultInterfaces;
    }

    pj::array ifaces = value_->at("listen_interfaces").get<pj::array>();
    std::vector<std::pair<std::string, int>> result;

    for (const pj::value &v : ifaces)
    {
        std::string net_addr = v.get<std::string>();
        size_t idx = net_addr.find_last_of(":");

        if (idx == 0)
        {
            continue;
        }

        std::string addr = net_addr.substr(0, idx);
        int port = std::stoi(net_addr.substr(idx + 1));
        result.push_back({ addr, port });
    }

    return result;
}

void configuration::set_listen_interfaces(const std::vector<std::pair<std::string, int>> &interfaces)
{
    pj::array ifaces;

    for (const std::pair<std::string, int> &v : interfaces)
    {
        std::stringstream ss;
        ss << v.first << ":" << v.second;
        ifaces.push_back(pj::value(ss.str()));
    }

    (*value_)["listen_interfaces"] = pj::value(ifaces);
}

bool configuration::prompt_for_remove_data()
{
    return get_or_default("prompt_for_remove_data", true);
}

void configuration::set_prompt_for_remove_data(bool value)
{
    set("prompt_for_remove_data", value);
}

bool configuration::prompt_for_save_path()
{
    return get_or_default("prompt_for_save_path", true);
}

void configuration::set_prompt_for_save_path(bool value)
{
    set("prompt_for_save_path", value);
}

configuration::proxy_type_t configuration::proxy_type()
{
    return (configuration::proxy_type_t)get_or_default("proxy_type", (int64_t)configuration::proxy_type_t::none);
}

void configuration::set_proxy_type(configuration::proxy_type_t type)
{
    set("proxy_type", (int)type);
}

std::string configuration::proxy_host()
{
    return get_or_default<std::string>("proxy_host", "");
}

void configuration::set_proxy_host(const std::string &host)
{
    set("proxy_host", host);
}

int configuration::proxy_port()
{
    return get_or_default("proxy_port", 0);
}

void configuration::set_proxy_port(int port)
{
    set("proxy_port", port);
}

std::string configuration::proxy_username()
{
    return get_or_default<std::string>("proxy_username", "");
}

void configuration::set_proxy_username(const std::string &username)
{
    set("proxy_username", username);
}

std::string configuration::proxy_password()
{
    return get_or_default<std::string>("proxy_password", "");
}

void configuration::set_proxy_password(const std::string &password)
{
    set("proxy_password", password);
}

bool configuration::proxy_force()
{
    return get_or_default("proxy_force", false);
}

void configuration::set_proxy_force(bool value)
{
    set("proxy_force", value);
}

bool configuration::proxy_hostnames()
{
    return get_or_default("proxy_hostnames", false);
}

void configuration::set_proxy_hostnames(bool value)
{
    set("proxy_hostnames", value);
}

bool configuration::proxy_peers()
{
    return get_or_default("proxy_peers", false);
}

void configuration::set_proxy_peers(bool value)
{
    set("proxy_peers", value);
}

bool configuration::proxy_trackers()
{
    return get_or_default("proxy_trackers", false);
}

void configuration::set_proxy_trackers(bool value)
{
    set("proxy_trackers", value);
}

std::shared_ptr<session_configuration> configuration::session_configuration()
{
    auto cfg = std::make_shared<core::session_configuration>();

    // Logging

    // Limits
    cfg->active_checking = session()->active_checking();
    cfg->active_dht_limit = session()->active_dht_limit();
    cfg->active_downloads = session()->active_downloads();
    cfg->active_limit = session()->active_limit();
    cfg->active_loaded_limit = session()->active_loaded_limit();
    cfg->active_lsd_limit = session()->active_lsd_limit();
    cfg->active_seeds = session()->active_seeds();
    cfg->active_tracker_limit = session()->active_tracker_limit();

    cfg->default_save_path = default_save_path();
    cfg->download_rate_limit = session()->download_rate_limit();
    cfg->enable_dht = true;
    cfg->listen_interfaces = listen_interfaces();
    cfg->session_state_file = pal::combine_paths(environment::get_data_path(), "Session.dat");
    cfg->stop_tracker_timeout = session()->stop_tracker_timeout();
    cfg->temporary_directory = environment::get_temporary_directory();
    cfg->torrents_directory = pal::combine_paths(environment::get_data_path(), "Torrents");
    cfg->upload_rate_limit = session()->upload_rate_limit();

    // Proxy
    cfg->proxy_force = proxy_force();
    cfg->proxy_host = proxy_host();
    cfg->proxy_hostnames = proxy_hostnames();
    cfg->proxy_peers = proxy_peers();
    cfg->proxy_port = proxy_port();
    cfg->proxy_trackers = proxy_trackers();
    cfg->proxy_type = static_cast<session_configuration::proxy_type_t>(proxy_type());

    return cfg;
}

configuration::start_position_t configuration::start_position()
{
    return (start_position_t)get_or_default("start_position", (int64_t)start_position_t::normal);
}

void configuration::set_start_position(configuration::start_position_t pos)
{
    set("start_position", (int64_t)pos);
}

std::string configuration::update_url()
{
    return get_or_default<std::string>("update_url", "https://api.github.com/repos/picotorrent/picotorrent/releases/latest");
}

template<typename T>
T configuration::get_or_default(const char *name, T defaultValue)
{
    auto &item = value_->find(name);

    if (item == value_->end())
    {
        return defaultValue;
    }

    return item->second.get<T>();
}

template<typename T>
void configuration::set(const char *name, T value)
{
    (*value_)[name] = pj::value(value);
}

template<>
int configuration::get_or_default<int>(const char *name, int defaultValue)
{
    auto &item = value_->find(name);

    if (item == value_->end())
    {
        return defaultValue;
    }

    return (int)item->second.get<int64_t>();
}

template<>
void configuration::set<int>(const char *name, int value)
{
    (*value_)[name] = pj::value((int64_t)value);
}

void configuration::load()
{
    std::string data_path = environment::get_data_path();
    std::string config_file = pal::combine_paths(data_path, "PicoTorrent.json");

    if (!pal::file_exists(config_file))
    {
        value_ = std::make_shared<pj::object>();
        return;
    }

    std::ifstream input(config_file, std::ios::binary);

    pj::value v;
    pj::parse(v, input);

    value_ = std::make_shared<pj::object>(v.get<pj::object>());
}

void configuration::save()
{
    pj::value v(*value_);
    std::string json = v.serialize(true);

    std::string data_path = environment::get_data_path();
    std::string config_file = pal::combine_paths(data_path, "PicoTorrent.json");

    std::ofstream output(config_file, std::ios::binary);
    output.write(json.c_str(), json.size());
}

std::shared_ptr<configuration::placement> configuration::window_placement(const std::string &name)
{
    if (value_->find("wnd_" + name) == value_->end())
    {
        return nullptr;
    }

    pj::value v = value_->at("wnd_" + name);

    if (!v.is<pj::array>())
    {
        return nullptr;
    }

    pj::array a = v.get<pj::array>();

    auto wp = std::make_shared<placement>();
    wp->flags = (uint32_t)a[0].get<int64_t>();
    wp->max_x = (long)a[1].get<int64_t>();
    wp->max_y = (long)a[2].get<int64_t>();
    wp->min_x = (long)a[3].get<int64_t>();
    wp->min_y = (long)a[4].get<int64_t>();
    wp->pos_bottom = (long)a[5].get<int64_t>();
    wp->pos_left = (long)a[6].get<int64_t>();
    wp->pos_right = (long)a[7].get<int64_t>();
    wp->pos_top = (long)a[8].get<int64_t>();
    wp->show = (uint32_t)a[9].get<int64_t>();

    return wp;
}

void configuration::set_window_placement(const std::string &name, const configuration::placement &wnd)
{
    pj::array a;
    a.push_back(pj::value((int64_t)wnd.flags));
    a.push_back(pj::value((int64_t)wnd.max_x));
    a.push_back(pj::value((int64_t)wnd.max_y));
    a.push_back(pj::value((int64_t)wnd.min_x));
    a.push_back(pj::value((int64_t)wnd.min_y));
    a.push_back(pj::value((int64_t)wnd.pos_bottom));
    a.push_back(pj::value((int64_t)wnd.pos_left));
    a.push_back(pj::value((int64_t)wnd.pos_right));
    a.push_back(pj::value((int64_t)wnd.pos_top));
    a.push_back(pj::value((int64_t)wnd.show));

    (*value_)["wnd_" + name] = pj::value(a);
}
