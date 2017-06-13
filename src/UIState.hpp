#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <libtorrent/entry.hpp>

class UIState
{
public:
    struct ListViewState
    {
        std::vector<int> order;
        std::vector<bool> visibility;
        std::vector<int> width;
    };

    struct WindowState
    {
        uint32_t flags;
        long max_x;
        long max_y;
        long min_x;
        long min_y;
        long pos_bottom;
        long pos_left;
        long pos_right;
        long pos_top;
        uint32_t show;
    };

    static UIState& GetInstance();

    ListViewState GetListViewState(const std::string& key);
    void SetListViewState(const std::string& key, const ListViewState& state);

    std::unique_ptr<WindowState> GetWindowState(const std::string& key);
    void SetWindowState(const std::string& key, const WindowState& state);

private:
    UIState();
    ~UIState();

    void Load();
    void Save();

    libtorrent::entry::dictionary_type m_map;
};
