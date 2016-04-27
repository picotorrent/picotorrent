#include <picotorrent/client/configuration.hpp>

#include <memory>
#include <string>

#include <picojson.hpp>

namespace pj = picojson;
using picotorrent::client::configuration;

configuration::part::part(const std::shared_ptr<pj::object> &cfg)
    : cfg_(cfg)
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

bool configuration::part::get_part_key_or_default(const char *part, const char* key, bool default_value)
{
    auto v = find_value(cfg_, part, key);
    return v == nullptr ? default_value : v->get<bool>();
}

int configuration::part::get_part_key_or_default(const char *part, const char* key, int default_value)
{
    auto v = find_value(cfg_, part, key);
    return v == nullptr ? default_value : (int)v->get<int64_t>();
}

std::string configuration::part::get_part_key_or_default(const char *part, const char* key, const std::string &default_value)
{
    auto v = find_value(cfg_, part, key);
    return v == nullptr ? default_value : v->get<std::string>();
}

void configuration::part::set_part_key(const char *part, const char* key, bool value)
{
    set_value(cfg_, part, key, pj::value(value));
}

void configuration::part::set_part_key(const char *part, const char* key, int value)
{
    set_value(cfg_, part, key, pj::value((int64_t)value));
}

void configuration::part::set_part_key(const char *part, const char* key, const std::string &value)
{
    set_value(cfg_, part, key, pj::value(value));
}
