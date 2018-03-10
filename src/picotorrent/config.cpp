#include "config.hpp"

#include <fstream>
#include <sstream>
#include <Windows.h>

#include "environment.hpp"

using pt::Configuration;

std::shared_ptr<Configuration> Configuration::Load(std::shared_ptr<pt::Environment> env, std::string& error)
{
    fs::path path = env->GetApplicationDataPath() / "PicoTorrent.json";

    if (!fs::exists(path)
        || !fs::is_regular_file(path))
    {
        return std::shared_ptr<Configuration>(new Configuration(env));
    }

    std::ifstream cfg_stream(path, std::ios::binary);

    picojson::value val;
    error = picojson::parse(val, cfg_stream);

    if (!val.is<picojson::object>())
    {
        return std::shared_ptr<Configuration>(new Configuration(env));
    }

    picojson::object obj = val.get<picojson::object>();
    auto obj_ptr = std::make_shared<picojson::object>(obj);

    return std::shared_ptr<Configuration>(new Configuration(env, obj_ptr));
}

void Configuration::Save(std::shared_ptr<pt::Environment> env, std::shared_ptr<Configuration> config)
{
    fs::path path = env->GetApplicationDataPath() / "PicoTorrent.json";
    picojson::value val(*(config->m_obj.get()));

    std::ofstream out(path, std::ios::binary | std::ios::out);
    val.serialize(std::ostreambuf_iterator<char>(out), true);
}

std::shared_ptr<Configuration::PresetsSection> Configuration::Presets()
{
    return std::shared_ptr<PresetsSection>(new PresetsSection(m_obj));
}

std::shared_ptr<Configuration::SessionSection> Configuration::Session()
{
    return std::shared_ptr<SessionSection>(new SessionSection(m_obj));
}

std::shared_ptr<Configuration::UISection> Configuration::UI()
{
    return std::shared_ptr<UISection>(new UISection(m_obj));
}

int Configuration::CurrentLanguageId()
{
    return Get<int>("language_id", static_cast<int>(GetUserDefaultUILanguage()));
}

void Configuration::CurrentLanguageId(int languageId)
{
    Set<int>("language_id", languageId);
}

fs::path Configuration::LanguagesPath()
{
    return Get<std::string>("languages_path", (m_env->GetApplicationDataPath() / "Languages").string());
}

fs::path Configuration::DefaultSavePath()
{
    return Get<std::string>("default_save_path", m_env->GetKnownFolderPath(Environment::KnownFolder::UserDownloads).string());
}

void Configuration::DefaultSavePath(fs::path path)
{
    Set("default_save_path", path.string());
}

std::vector<std::pair<std::string, int>> Configuration::ListenInterfaces()
{
    std::vector<std::pair<std::string, int>> defaultInterfaces = {
        { "0.0.0.0", 6881 },
        { "[::]", 6881 }
    };

    if (m_obj->find("listen_interfaces") == m_obj->end())
    {
        return defaultInterfaces;
    }

    picojson::array ifaces = m_obj->at("listen_interfaces").get<picojson::array>();
    std::vector<std::pair<std::string, int>> result;

    for (const picojson::value &v : ifaces)
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

void Configuration::ListenInterfaces(const std::vector<std::pair<std::string, int>>& interfaces)
{
    picojson::array ifaces;

    for (const std::pair<std::string, int> &v : interfaces)
    {
        std::stringstream ss;
        ss << v.first << ":" << v.second;
        ifaces.push_back(picojson::value(ss.str()));
    }

    (*m_obj)["listen_interfaces"] = picojson::value(ifaces);
}

Configuration::ConnectionProxyType Configuration::ProxyType()
{
    return static_cast<ConnectionProxyType>(Get("proxy_type", static_cast<int64_t>(ConnectionProxyType::None)));
}

void Configuration::ProxyType(ConnectionProxyType type)
{
    Set("proxy_type", static_cast<int64_t>(type));
}

std::string Configuration::ProxyHost()
{
    return Get<std::string>("proxy_host", "");
}

void Configuration::ProxyHost(const std::string& host)
{
    Set("proxy_host", host);
}

int Configuration::ProxyPort()
{
    return Get("proxy_port", 0);
}

void Configuration::ProxyPort(int port)
{
    Set("proxy_port", port);
}

std::string Configuration::ProxyUsername()
{
    return Get<std::string>("proxy_username", "");
}

void Configuration::ProxyUsername(const std::string& username)
{
    Set("proxy_username", username);
}

std::string Configuration::ProxyPassword()
{
    return Get<std::string>("proxy_password", "");
}

void Configuration::ProxyPassword(const std::string& password)
{
    Set("proxy_password", password);
}

bool Configuration::ProxyForce()
{
    return Get("proxy_force", false);
}

void Configuration::ProxyForce(bool force)
{
    Set("proxy_force", force);
}

bool Configuration::ProxyHostnames()
{
    return Get("proxy_hostnames", false);
}

void Configuration::ProxyHostnames(bool b)
{
    Set("proxy_hostnames", b);
}

bool Configuration::ProxyPeers()
{
    return Get("proxy_peers", false);
}

void Configuration::ProxyPeers(bool b)
{
    Set("proxy_peers", b);
}

bool Configuration::ProxyTrackers()
{
    return Get("proxy_trackers", false);
}

void Configuration::ProxyTrackers(bool b)
{
    Set("proxy_trackers", b);
}

Configuration::WindowState Configuration::StartPosition()
{
    return static_cast<WindowState>(Get("start_position", static_cast<int64_t>(WindowState::Normal)));
}

void Configuration::StartPosition(Configuration::WindowState state)
{
    Set<int>("start_position", static_cast<int>(state));
}

Configuration::Configuration(std::shared_ptr<pt::Environment> env, std::shared_ptr<picojson::object> obj)
    : m_env(env),
    m_obj(obj)
{
}

template<typename T>
T Configuration::Get(const char *name, T defaultValue)
{
    auto &item = m_obj->find(name);
    if (item == m_obj->end()) { return defaultValue; }
    return item->second.get<T>();
}

template<typename T>
void Configuration::Set(const char *name, T value)
{
    (*m_obj)[name] = picojson::value(value);
}

template<>
int Configuration::Get<int>(const char *name, int defaultValue)
{
    auto &item = m_obj->find(name);
    if (item == m_obj->end()) { return defaultValue; }
    return (int)item->second.get<int64_t>();
}

template<>
void Configuration::Set<int>(const char *name, int value)
{
    (*m_obj)[name] = picojson::value((int64_t)value);
}
