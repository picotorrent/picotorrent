#include "UIState.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <libtorrent/bdecode.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>

#include "Environment.hpp"

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;

UIState& UIState::GetInstance()
{
    static UIState instance;
    return instance;
}

UIState::ListViewState UIState::GetListViewState(const std::string& key)
{
    if (m_map.find("listViews") == m_map.end()
        || m_map.at("listViews").type() != lt::entry::data_type::dictionary_t)
    {
        return ListViewState();
    }

    lt::entry::dictionary_type lvs = m_map.at("listViews").dict();

    if (lvs.find(key) == lvs.end()
        || lvs.at(key).type() != lt::entry::data_type::dictionary_t)
    {
        return ListViewState();
    }

    lt::entry::dictionary_type list = lvs.at(key).dict();

    ListViewState state;

    if (list.find("order") != list.end() && list.at("order").type() == lt::entry::data_type::list_t)
    {
        lt::entry::list_type orderList = list.at("order").list();

        for each (auto le in orderList)
        {
            state.order.push_back(static_cast<int>(le.integer()));
        }
    }

    if (list.find("visibility") != list.end() && list.at("visibility").type() == lt::entry::data_type::list_t)
    {
        lt::entry::list_type visibilityList = list.at("visibility").list();

        for each (auto le in visibilityList)
        {
            state.visibility.push_back(le.integer() > 0);
        }
    }

    if (list.find("width") != list.end() && list.at("width").type() == lt::entry::data_type::list_t)
    {
        lt::entry::list_type widthList = list.at("width").list();

        for each (auto le in widthList)
        {
            state.width.push_back(static_cast<int>(le.integer()));
        }
    }

    return state;
}

void UIState::SetListViewState(const std::string& key, const UIState::ListViewState& state)
{
    lt::entry::list_type orderList;
    lt::entry::list_type visibilityList;
    lt::entry::list_type widthList;

    for (size_t i = 0; i < state.order.size(); i++)
    {
        orderList.push_back(state.order.at(i));
    }

    for (size_t i = 0; i < state.visibility.size(); i++)
    {
        visibilityList.push_back(state.visibility.at(i) ? 1 : 0);
    }

    for (size_t i = 0; i < state.width.size(); i++)
    {
        widthList.push_back(state.width.at(i));
    }

    lt::entry::dictionary_type list;
    list.insert({ "order", orderList });
    list.insert({ "visibility", visibilityList });
    list.insert({ "width", widthList });

    if (m_map.find("listViews") == m_map.end())
    {
        m_map.insert({ "listViews", lt::entry(lt::entry::dictionary_type()) });
    }

    lt::entry::dictionary_type& listViews = m_map.at("listViews").dict();
    listViews[key] = list;
}

std::unique_ptr<UIState::WindowState> UIState::GetWindowState(const std::string& key)
{
    // Find wnd
    if (m_map.find("wnd") == m_map.end()) { return nullptr; }

    lt::entry wnd = m_map.at("wnd");

    if (wnd.type() != lt::entry::data_type::dictionary_t)
    {
        // LOG (perhaps data corruption?)
        return nullptr;
    }

    lt::entry::dictionary_type wnd_map = wnd.dict();

    if (wnd_map.find(key) == wnd_map.end())
    {
        return nullptr;
    }

    lt::entry wnd_entry = wnd_map.at(key);

    if (wnd_entry.type() != lt::entry::data_type::dictionary_t)
    {
        return nullptr;
    }

    lt::entry::dictionary_type dict = wnd_entry.dict();

    std::unique_ptr<WindowState> state = std::make_unique<WindowState>();
    state->flags = static_cast<uint32_t>(dict.at("flags").integer());
    state->max_x = static_cast<int>(dict.at("max_x").integer());
    state->max_y = static_cast<int>(dict.at("max_y").integer());
    state->min_x = static_cast<int>(dict.at("min_x").integer());
    state->min_y = static_cast<int>(dict.at("min_y").integer());
    state->pos_bottom = static_cast<int>(dict.at("pos_bottom").integer());
    state->pos_left = static_cast<int>(dict.at("pos_left").integer());
    state->pos_right = static_cast<int>(dict.at("pos_right").integer());
    state->pos_top = static_cast<int>(dict.at("pos_top").integer());
    state->show = static_cast<uint32_t>(dict.at("show").integer());

    return state;
}

void UIState::SetWindowState(const std::string& key, const UIState::WindowState& state)
{
    // Find wnd
    if (m_map.find("wnd") == m_map.end())
    {
        m_map.insert({ "wnd", lt::entry(lt::entry::data_type::dictionary_t) });
    }

    lt::entry& wnd_entry = m_map.at("wnd");
    if (wnd_entry.type() != lt::entry::data_type::dictionary_t) { return; }

    lt::entry::dictionary_type s;
    s.insert({ "flags", state.flags });
    s.insert({ "max_x", state.max_x });
    s.insert({ "max_y", state.max_y });
    s.insert({ "min_x", state.min_x });
    s.insert({ "min_y", state.min_y });
    s.insert({ "pos_bottom", state.pos_bottom });
    s.insert({ "pos_left", state.pos_left });
    s.insert({ "pos_right", state.pos_right });
    s.insert({ "pos_top", state.pos_top });
    s.insert({ "show", state.show });

    wnd_entry[key] = lt::entry(s);
}

UIState::UIState()
{
    Load();
}

UIState::~UIState()
{
    Save();
}

void UIState::Load()
{
    fs::path data_path = Environment::GetDataPath();
    fs::path state_file = data_path / "UI.dat";

    if (!fs::exists(state_file))
    {
        return;
    }

    std::ifstream input(state_file, std::ios::binary);
    std::stringstream ss;
    ss << input.rdbuf();
    std::string buf = ss.str();

    lt::bdecode_node node;
    lt::error_code ltec;

    lt::bdecode(&buf[0], &buf[0] + buf.size(),node, ltec);

    if (ltec)
    {
        // LOG
        return;
    }


    lt::entry e;
    e = node;

    if (e.type() != lt::entry::data_type::dictionary_t)
    {
        return;
    }

    m_map = e.dict();
}

void UIState::Save()
{
    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), lt::entry(m_map));

    fs::path data_path = Environment::GetDataPath();
    fs::path state_file = data_path / "UI.dat";

    std::ofstream out(state_file, std::ios::binary | std::ios::out);
    std::copy(
        buf.begin(),
        buf.end(),
        std::ostreambuf_iterator<char>(out));
}
