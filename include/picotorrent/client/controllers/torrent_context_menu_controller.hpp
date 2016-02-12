#pragma once

#include <memory>
#include <vector>
#include <string>
#include <windows.h>

namespace picotorrent
{
namespace core
{
    class session;
    class torrent;
}
namespace client
{
namespace ui
{
    class main_window;
}
namespace controllers
{
    class torrent_context_menu_controller
    {
    public:
        explicit torrent_context_menu_controller(const std::shared_ptr<core::session> &session,
            const std::vector<std::shared_ptr<core::torrent>> &torrents,
            const std::shared_ptr<ui::main_window> &wnd);

        void execute(const POINT &p);

    private:
        void copy_to_clipboard(const std::string &text);
        void open_and_select_item(const std::wstring &path, const std::wstring &item);

        std::shared_ptr<core::session> sess_;
        std::vector<std::shared_ptr<core::torrent>> torrents_;
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
