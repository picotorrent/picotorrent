#pragma once

#include <memory>

namespace picotorrent
{
namespace core
{
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
    class move_torrent_controller
    {
    public:
        explicit move_torrent_controller(const std::shared_ptr<ui::main_window> &wnd, const std::shared_ptr<core::torrent> &torrent);
        ~move_torrent_controller();

        void execute();

    private:
        std::shared_ptr<ui::main_window> wnd_;
        std::shared_ptr<core::torrent> torrent_;
    };
}
}
}