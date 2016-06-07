#include <picotorrent/common/config/configuration.hpp>

#include <memory>

#include <picojson.hpp>

namespace pj = picojson;
using picotorrent::common::config::configuration;

std::map<int, configuration::ui_part::list_view_column_state> configuration::ui_part::list_view_state(const std::string &id)
{
    std::map<int, configuration::ui_part::list_view_column_state> state;

    if (cfg_->find("list_view_state") == cfg_->end())
    {
        return state;
    }

    pj::object list_state = cfg_->at("list_view_state").get<pj::object>();

    if (list_state.find(id) == list_state.end())
    {
        return state;
    }

    pj::array arr = list_state.at(id).get<pj::array>();

    for (auto &i : arr)
    {
        ui_part::list_view_column_state s;
        s.visible = i.get<pj::object>()["visible"].get<bool>();
        s.width = (int)i.get<pj::object>()["width"].get<int64_t>();

        int id = (int)i.get<pj::object>()["id"].get<int64_t>();

        state.insert({ id, s });
    }

    return state;
}

void configuration::ui_part::list_view_state(const std::string &id, const std::map<int, configuration::ui_part::list_view_column_state> &state)
{
    pj::object list_state;

    if (cfg_->find("list_view_state") != cfg_->end())
    {
        list_state = cfg_->at("list_view_state").get<pj::object>();
    }

    pj::array arr;

    for (auto &p : state)
    {
        pj::object o;
        o.insert({ "id", pj::value((int64_t)p.first) });
        o.insert({ "visible", pj::value(p.second.visible) });
        o.insert({ "width", pj::value((int64_t)p.second.width) });

        arr.push_back(pj::value(o));
    }

    list_state[id] = pj::value(arr);
    (*cfg_)["list_view_state"] = pj::value(list_state);
}
