#pragma once

#include <memory>
#include <vector>

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
        explicit move_torrent_controller(const std::shared_ptr<ui::main_window> &wnd, const std::vector<std::shared_ptr<core::torrent>> &torrents);
        ~move_torrent_controller();

        void execute();

    private:
        std::shared_ptr<ui::main_window> wnd_;
        std::vector<std::shared_ptr<core::torrent>> torrents_;
    };
}
}
}