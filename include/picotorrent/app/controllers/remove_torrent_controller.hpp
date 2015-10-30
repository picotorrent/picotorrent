#pragma once

#include <memory>

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
    class remove_torrent_controller
    {
    public:
        explicit remove_torrent_controller(
            const std::shared_ptr<ui::main_window> &wnd,
            const std::shared_ptr<core::session> &session,
            const std::shared_ptr<core::torrent> &torrent);

        ~remove_torrent_controller();

        void execute();

    private:
        std::shared_ptr<ui::main_window> wnd_;
        std::shared_ptr<core::session> session_;
        std::shared_ptr<core::torrent> torrent_;
    };
}
}
}