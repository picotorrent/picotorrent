#include "UIState.hpp"

#include <string>
#include <vector>

#include <libtorrent/bdecode.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>

#include "Environment.hpp"
#include "IO/File.hpp"
#include "IO/Path.hpp"

namespace lt = libtorrent;

UIState& UIState::GetInstance()
{
    static UIState instance;
    return instance;
}

UIState::ColumnStateMap UIState::GetListViewColumnState(const std::string& key)
{
    // Find list view
    if (m_map.find("lv") == m_map.end())
    {
        return ColumnStateMap();
    }

    lt::entry lv = m_map.at("lv");

    if (lv.type() != lt::entry::data_type::dictionary_t)
    {
        // LOG (perhaps data corruption?)
        return ColumnStateMap();
    }

    std::map<std::string, lt::entry> lv_map = lv.dict();

    if (lv_map.find(key) == lv_map.end())
    {
        return ColumnStateMap();
    }

    lt::entry lv_entry = lv_map.at(key);

    if (lv_entry.type() != lt::entry::data_type::list_t)
    {
        return ColumnStateMap();
    }

    std::vector<lt::entry> lv_entry_list = lv_entry.list();
    ColumnStateMap m;

    for (lt::entry& e : lv_entry_list)
    {
        if (e.type() != lt::entry::data_type::dictionary_t)
        {
            return ColumnStateMap();
        }

        std::map<std::string, lt::entry> emap = e.dict();

        ColumnState st;
        int id = static_cast<int>(emap.at("id").integer());

        st.order = static_cast<int>(emap.at("o").integer());
        st.visible = static_cast<int>(emap.at("v").integer()) > 0 ? true : false;
        st.width = static_cast<int>(emap.at("w").integer());

        m.insert({ id, st });
    }

    return m;
}

void UIState::SetListViewColumnState(const std::string& key, const ColumnStateMap& map)
{
    // Find list view
    if (m_map.find("lv") == m_map.end())
    {
        m_map.insert({ "lv", lt::entry(lt::entry::data_type::dictionary_t) });
    }

    lt::entry& lv_entry = m_map.at("lv");
    if (lv_entry.type() != lt::entry::data_type::dictionary_t) { return; }

    std::vector<lt::entry> l;

    for (auto& p : map)
    {
        std::map<std::string, lt::entry> o;
        o.insert({ "id", lt::entry(p.first) });
        o.insert({ "o", lt::entry(p.second.order) });
        o.insert({ "v", lt::entry(p.second.visible ? 1 : 0) });
        o.insert({ "w", lt::entry(p.second.width) });

        l.push_back(lt::entry(o));
    }

    lv_entry[key] = lt::entry(l);
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

    std::map<std::string, lt::entry> wnd_map = wnd.dict();

    if (wnd_map.find(key) == wnd_map.end())
    {
        return nullptr;
    }

    lt::entry wnd_entry = wnd_map.at(key);

    if (wnd_entry.type() != lt::entry::data_type::dictionary_t)
    {
        return nullptr;
    }

    std::map<std::string, lt::entry> dict = wnd_entry.dict();

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

    std::map<std::string, lt::entry> s;
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
    std::wstring data_path = Environment::GetDataPath();
    std::wstring state_file = IO::Path::Combine(data_path, TEXT("UI.dat"));

    if (!IO::File::Exists(state_file))
    {
        return;
    }

    std::error_code ec;
    std::vector<char> buf = IO::File::ReadAllBytes(state_file, ec);

    if (ec)
    {
        // LOG
        return;
    }

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

    std::wstring data_path = Environment::GetDataPath();
    std::wstring state_file = IO::Path::Combine(data_path, TEXT("UI.dat"));

    std::error_code ec;
    IO::File::WriteAllBytes(state_file, buf, ec);
}
