#include "Configuration.hpp"

#include <picojson.hpp>
#include <sstream>
#include <windows.h>

#include "Environment.hpp"
#include "IO/File.hpp"
#include "IO/Path.hpp"
#include "StringUtils.hpp"

namespace pj = picojson;

const LPTSTR FileName = TEXT("PicoTorrent.json");

Configuration::Configuration()
{
    Load();
}

Configuration::~Configuration()
{
    Save();
}

Configuration& Configuration::GetInstance()
{
    static Configuration instance;
    return instance;
}

std::shared_ptr<pj::object> Configuration::GetRawObject()
{
    return m_cfg;
}

std::shared_ptr<Configuration::SessionSection> Configuration::Session()
{
    return std::shared_ptr<SessionSection>(new SessionSection(m_cfg));
}

std::shared_ptr<Configuration::UISection> Configuration::UI()
{
    return std::shared_ptr<UISection>(new UISection(m_cfg));
}

Configuration::CloseAction Configuration::GetCloseAction()
{
    return (CloseAction)Get("close_action", (int64_t)CloseAction::Prompt);
}

void Configuration::SetCloseAction(Configuration::CloseAction action)
{
    Set("close_action", (int64_t)action);
}

int Configuration::GetCurrentLanguageId()
{
    return Get("language_id", (int)GetUserDefaultLangID());
}

void Configuration::SetCurrentLanguageId(int languageId)
{
    Set("language_id", languageId);
}

std::string Configuration::GetDefaultSavePath()
{
    return Get<std::string>("default_save_path",
        ToString(Environment::GetKnownFolderPath(FOLDERID_Downloads)));
}

void Configuration::SetDefaultSavePath(const std::string& path)
{
    Set("default_save_path", path);
}

std::vector<std::pair<std::string, std::string>> Configuration::GetFileFilters()
{
    if (m_cfg->find("file_filters") == m_cfg->end()
        || !m_cfg->at("file_filters").is<pj::array>())
    {
        return std::vector<std::pair<std::string, std::string>>();
    }

    pj::array& arr = m_cfg->at("file_filters").get<pj::array>();
    std::vector<std::pair<std::string, std::string>> filters;

    for (pj::value& val : arr)
    {
        if (!val.is<pj::object>())
        {
            continue;
        }

        pj::object& obj = val.get<pj::object>();
        
        if (obj.find("name") == obj.end()
            && obj.find("pattern") == obj.end())
        {
            continue;
        }

        if (!obj.at("name").is<std::string>()
            || !obj.at("pattern").is<std::string>())
        {
            continue;
        }

        std::string name = obj.at("name").get<std::string>();
        std::string pattern = obj.at("pattern").get<std::string>();

        filters.push_back({ name, pattern });
    }

    return filters;
}

bool Configuration::GetMoveCompletedDownloads()
{
    return Get("move_completed_downloads", false);
}

void Configuration::SetMoveCompletedDownloads(bool value)
{
    Set("move_completed_downloads", value);
}

std::string Configuration::GetMoveCompletedDownloadsPath()
{
    return Get<std::string>("move_completed_downloads_path", "");
}

void Configuration::SetMoveCompletedDownloadsPath(const std::string& path)
{
    Set("move_completed_downloads_path", path);
}

bool Configuration::GetMoveCompletedDownloadsFromDefaultOnly()
{
    return Get("move_completed_downloads_from_default_only", false);
}

void Configuration::SetMoveCompletedDownloadsFromDefaultOnly(bool value)
{
    Set("move_completed_downloads_from_default_only", value);
}

std::vector<std::pair<std::string, int>> Configuration::GetListenInterfaces()
{
    std::vector<std::pair<std::string, int>> defaultInterfaces = {
        { "0.0.0.0", 6881 },
        { "[::]", 6881 }
    };

    if (m_cfg->find("listen_interfaces") == m_cfg->end())
    {
        return defaultInterfaces;
    }

    pj::array ifaces = m_cfg->at("listen_interfaces").get<pj::array>();
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

void Configuration::SetListenInterfaces(const std::vector<std::pair<std::string, int>>& interfaces)
{
    pj::array ifaces;

    for (const std::pair<std::string, int> &v : interfaces)
    {
        std::stringstream ss;
        ss << v.first << ":" << v.second;
        ifaces.push_back(pj::value(ss.str()));
    }

    (*m_cfg)["listen_interfaces"] = pj::value(ifaces);
}

bool Configuration::GetPromptForRemovingData()
{
    return Get("prompt_for_remove_data", true);
}

void Configuration::SetPromptForRemovingData(bool val)
{
    Set("prompt_for_remove_data", val);
}

Configuration::ProxyType Configuration::GetProxyType()
{
    return (ProxyType)Get("proxy_type", (int64_t)ProxyType::None);
}

void Configuration::SetProxyType(ProxyType type)
{
    Set("proxy_type", (int64_t)type);
}

std::string Configuration::GetProxyHost()
{
    return Get<std::string>("proxy_host", "");
}

void Configuration::SetProxyHost(const std::string& host)
{
    Set("proxy_host", host);
}

int Configuration::GetProxyPort()
{
    return Get("proxy_port", 0);
}

void Configuration::SetProxyPort(int port)
{
    Set("proxy_port", port);
}

std::string Configuration::GetProxyUsername()
{
    return Get<std::string>("proxy_username", "");
}

void Configuration::SetProxyUsername(const std::string& username)
{
    Set("proxy_username", username);
}

std::string Configuration::GetProxyPassword()
{
    return Get<std::string>("proxy_password", "");
}

void Configuration::SetProxyPassword(const std::string& password)
{
    Set("proxy_password", password);
}

bool Configuration::GetProxyForce()
{
    return Get("proxy_force", false);
}

void Configuration::SetProxyForce(bool force)
{
    Set("proxy_force", force);
}

bool Configuration::GetProxyHostnames()
{
    return Get("proxy_hostnames", false);
}

void Configuration::SetProxyHostnames(bool b)
{
    Set("proxy_hostnames", b);
}

bool Configuration::GetProxyPeers()
{
    return Get("proxy_peers", false);
}

void Configuration::SetProxyPeers(bool b)
{
    Set("proxy_peers", b);
}

bool Configuration::GetProxyTrackers()
{
    return Get("proxy_trackers", false);
}

void Configuration::SetProxyTrackers(bool b)
{
    Set("proxy_trackers", b);
}

Configuration::StartupPosition Configuration::GetStartupPosition()
{
    return (StartupPosition)Get("start_position", (int64_t)StartupPosition::Normal);
}

void Configuration::SetStartupPosition(Configuration::StartupPosition pos)
{
    Set("start_position", (int64_t)pos);
}

template<typename T>
T Configuration::Get(const char *name, T defaultValue)
{
    auto &item = m_cfg->find(name);
    if (item == m_cfg->end()) { return defaultValue; }
    return item->second.get<T>();
}

template<typename T>
void Configuration::Set(const char *name, T value)
{
    (*m_cfg)[name] = pj::value(value);
}

template<>
int Configuration::Get<int>(const char *name, int defaultValue)
{
    auto &item = m_cfg->find(name);
    if (item == m_cfg->end()) { return defaultValue; }
    return (int)item->second.get<int64_t>();
}

template<>
void Configuration::Set<int>(const char *name, int value)
{
    (*m_cfg)[name] = pj::value((int64_t)value);
}

void Configuration::Load()
{
    std::wstring data_path = Environment::GetDataPath();
    std::wstring config_file = IO::Path::Combine(data_path, FileName);

    if (!IO::File::Exists(config_file))
    {
        m_cfg = std::make_shared<pj::object>();
        return;
    }

    std::error_code ec;
    std::vector<char> buf = IO::File::ReadAllBytes(config_file, ec);

    if (ec)
    {
        m_cfg = std::make_shared<pj::object>();
        return;
    }

    pj::value v;
    pj::parse(v, buf.begin(), buf.end());

    if (!v.is<pj::object>())
    {
        m_cfg = std::make_shared<pj::object>();
        return;
    }

    m_cfg = std::make_shared<pj::object>(v.get<pj::object>());
}

void Configuration::Save()
{
    pj::value v(*m_cfg);

    std::vector<char> buf;
    v.serialize(std::back_inserter(buf), true);

    std::wstring data_path = Environment::GetDataPath();
    std::wstring config_file = IO::Path::Combine(data_path, FileName);

    std::error_code ec;
    IO::File::WriteAllBytes(config_file, buf, ec);
}
