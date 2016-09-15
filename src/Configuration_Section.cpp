#include "Configuration.hpp"

#include <memory>
#include <string>

#include <picojson.hpp>

namespace pj = picojson;

Configuration::Section::Section(const std::shared_ptr<pj::object>& part)
    : m_part(part)
{
}

std::shared_ptr<pj::value> find_value(const std::shared_ptr<pj::object> &cfg, const char *part, const char* key)
{
    if (cfg->find(part) == cfg->end()) { return nullptr; }

    pj::value v = cfg->at(part);
    if (!v.is<pj::object>()) { return nullptr; }

    pj::object o = v.get<pj::object>();
    if (o.find(key) == o.end()) { return nullptr; }

    return std::make_shared<pj::value>(o.at(key));
}

void set_value(const std::shared_ptr<pj::object> &cfg, const char *part, const char* key, const pj::value &val)
{
    if (cfg->find(part) == cfg->end())
    {
        pj::object temp;
        cfg->insert({ part, pj::value(temp) });
    }

    pj::object o = cfg->at(part).get<pj::object>();
    o[key] = val;
    (*cfg.get())[part] = pj::value(o);
}

bool Configuration::Section::Get(const char *part, const char* key, bool default_value)
{
    auto v = find_value(m_part, part, key);
    return v == nullptr ? default_value : v->get<bool>();
}

int Configuration::Section::Get(const char *part, const char* key, int default_value)
{
    auto v = find_value(m_part, part, key);
    return v == nullptr ? default_value : (int)v->get<int64_t>();
}

std::string Configuration::Section::Get(const char *part, const char* key, const std::string &default_value)
{
    auto v = find_value(m_part, part, key);
    return v == nullptr ? default_value : v->get<std::string>();
}

void Configuration::Section::Set(const char *part, const char* key, bool value)
{
    set_value(m_part, part, key, pj::value(value));
}

void Configuration::Section::Set(const char *part, const char* key, int value)
{
    set_value(m_part, part, key, pj::value((int64_t)value));
}

void Configuration::Section::Set(const char *part, const char* key, const std::string &value)
{
    set_value(m_part, part, key, pj::value(value));
}
