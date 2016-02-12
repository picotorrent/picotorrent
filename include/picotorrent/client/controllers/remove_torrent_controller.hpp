#pragma once

#include <memory>
#include <vector>

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
    class remove_torrent_controller
    {
    public:
        explicit remove_torrent_controller(
            const std::shared_ptr<ui::main_window> &wnd,
            const std::shared_ptr<core::session> &session,
            const std::vector<std::shared_ptr<core::torrent>> &torrent);

        ~remove_torrent_controller();

        void execute(bool remove_files = false);

    private:
        std::shared_ptr<ui::main_window> wnd_;
        std::shared_ptr<core::session> session_;
        std::vector<std::shared_ptr<core::torrent>> torrents_;
    };
}
}
}