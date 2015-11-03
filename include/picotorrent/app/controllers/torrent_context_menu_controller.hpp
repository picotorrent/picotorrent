#pragma once

#include <memory>
#include <string>
#include <windows.h>

namespace picotorrent
{
namespace core
{
    class session;
    class torrent;
}
namespace ui
{
    class main_window;
}
namespace app
{
namespace controllers
{
    class torrent_context_menu_controller
    {
    public:
        explicit torrent_context_menu_controller(const std::shared_ptr<core::session> &session,
            const std::shared_ptr<core::torrent> &torrent,
            const std::shared_ptr<ui::main_window> &wnd);

        void execute(const POINT &p);

    private:
        void copy_to_clipboard(const std::string &text);

        std::shared_ptr<core::session> sess_;
        std::shared_ptr<core::torrent> torrent_;
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
