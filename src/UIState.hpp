#pragma once

#include <map>
#include <memory>
#include <string>

namespace libtorrent
{
    class entry;
}

class UIState
{
public:
    struct ColumnState
    {
        int order;
        int width;
        bool visible;
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

    typedef std::map<int, ColumnState> ColumnStateMap;

    static UIState& GetInstance();

    ColumnStateMap GetListViewColumnState(const std::string& key);
    void SetListViewColumnState(const std::string& key, const ColumnStateMap& state);

    std::unique_ptr<WindowState> GetWindowState(const std::string& key);
    void SetWindowState(const std::string& key, const WindowState& state);

private:
    UIState();
    ~UIState();

    void Load();
    void Save();

    std::map<std::string, libtorrent::entry> m_map;
};
