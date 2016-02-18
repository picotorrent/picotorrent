#include <picotorrent/core/configuration.hpp>

#include <picojson.hpp>
#include <picotorrent/core/environment.hpp>
#include <picotorrent/core/string_operations.hpp>
#include <picotorrent/core/filesystem/directory.hpp>
#include <picotorrent/core/filesystem/file.hpp>
#include <picotorrent/core/filesystem/path.hpp>

#include <windows.h>

namespace fs = picotorrent::core::filesystem;
namespace pj = picojson;
using picotorrent::core::to_wstring;
using picotorrent::core::configuration;

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

int configuration::alert_queue_size()
{
    return get_or_default("alert_queue_size", 500);
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

std::wstring configuration::default_save_path()
{
    fs::path defaultPath = environment::get_special_folder(special_folder::user_downloads);
    return get_or_default<std::wstring>("default_save_path", defaultPath.to_string());
}

void configuration::set_default_save_path(const std::wstring &path)
{
    set("default_save_path", path);
}

int configuration::download_rate_limit()
{
    return get_or_default("global_dl_rate_limit", 0);
}

void configuration::set_download_rate_limit(int dl_rate)
{
    set("global_dl_rate_limit", dl_rate);
}

std::wstring configuration::ignored_update()
{
    return get_or_default<std::wstring>("ignored_update", L"");
}

void configuration::set_ignored_update(const std::wstring &version)
{
    set("ignored_update", version);
}

std::wstring configuration::listen_address()
{
    return get_or_default<std::wstring>("listen_address", L"0.0.0.0");
}

void configuration::set_listen_address(const std::wstring &address)
{
    set("listen_address", address);
}

int configuration::listen_port()
{
    return get_or_default("listen_port", 6881);
}

void configuration::set_listen_port(int port)
{
    set("listen_port", port);
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

std::wstring configuration::proxy_host()
{
    return get_or_default<std::wstring>("proxy_host", L"");
}

void configuration::set_proxy_host(const std::wstring &host)
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

std::wstring configuration::proxy_username()
{
    return get_or_default<std::wstring>("proxy_username", L"");
}

void configuration::set_proxy_username(const std::wstring &username)
{
    set("proxy_username", username);
}

std::wstring configuration::proxy_password()
{
    return get_or_default<std::wstring>("proxy_password", L"");
}

void configuration::set_proxy_password(const std::wstring &password)
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

int configuration::stop_tracker_timeout()
{
    return get_or_default("stop_tracker_timeout", 1);
}

std::wstring configuration::update_url()
{
    return get_or_default<std::wstring>("update_url", L"https://api.github.com/repos/picotorrent/picotorrent/releases/latest");
}

int configuration::upload_rate_limit()
{
    return get_or_default("global_ul_rate_limit", 0);
}

void configuration::set_upload_rate_limit(int ul_rate)
{
    set("global_ul_rate_limit", ul_rate);
}

bool configuration::use_picotorrent_peer_id()
{
    return get_or_default("use_picotorrent_peer_id", false);
}

void configuration::set_use_picotorrent_peer_id(bool value)
{
    set("use_picotorrent_peer_id", value);
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
std::wstring configuration::get_or_default<std::wstring>(const char *name, std::wstring defaultValue)
{
    auto &item = value_->find(name);

    if (item == value_->end())
    {
        return defaultValue;
    }

    return to_wstring(item->second.get<std::string>());
}

template<>
void configuration::set<int>(const char *name, int value)
{
    (*value_)[name] = pj::value((int64_t)value);
}

template<>
void configuration::set<std::wstring>(const char *name, std::wstring value)
{
    std::string s = to_string(value);
    if (!s.empty() && s[s.size() - 1] == '\0') { s = s.substr(0, s.size() - 1); }

    (*value_)[name] = pj::value(s);
}

void configuration::load()
{
    fs::path data = environment::get_data_path();
    fs::file cfg = data.combine(L"PicoTorrent.json");

    if (!cfg.path().exists())
    {
        value_ = std::make_unique<pj::object>();
        return;
    }

    std::vector<char> buf;
    cfg.read_all(buf);

    pj::value v;
    pj::parse(v, buf.begin(), buf.end(), nullptr);

    value_ = std::make_unique<pj::object>(v.get<pj::object>());
}

void configuration::save()
{
    pj::value v(*value_);
    std::string json = v.serialize(true);
    std::vector<char> buf(json.begin(), json.end());

    fs::directory data = environment::get_data_path();

    if (!data.path().exists())
    {
        data.create();
    }

    fs::file cfg = data.path().combine(L"PicoTorrent.json");
    cfg.write_all(buf);
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
