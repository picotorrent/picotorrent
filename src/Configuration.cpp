#include "Configuration.hpp"

#include <picojson.hpp>

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

std::shared_ptr<Configuration::SessionSection> Configuration::Session()
{
    return std::shared_ptr<SessionSection>(new SessionSection(m_cfg));
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
