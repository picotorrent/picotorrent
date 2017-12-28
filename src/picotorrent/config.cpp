#include "config.hpp"

#include <fstream>
#include <sstream>
#include <Windows.h>

#include "environment.hpp"

using pt::Configuration;

std::shared_ptr<Configuration> Configuration::Load(std::shared_ptr<pt::Environment> env)
{
    fs::path path = env->GetApplicationDataPath() / "PicoTorrent.json";

    if (!fs::exists(path)
        || !fs::is_regular_file(path))
    {
        return std::shared_ptr<Configuration>(new Configuration(env));
    }

    std::ifstream cfg_stream(path, std::ios::binary);

    picojson::value val;
    picojson::parse(val, cfg_stream);

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

int Configuration::CurrentLanguageId()
{
    return Get<int>("language_id", static_cast<int>(GetUserDefaultUILanguage()));
}

fs::path Configuration::DefaultSavePath()
{
    return Get<std::string>("default_save_path", m_env->GetKnownFolderPath(Environment::KnownFolder::UserDownloads).string());
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
    (*m_obj)[name] = pj::value(value);
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
