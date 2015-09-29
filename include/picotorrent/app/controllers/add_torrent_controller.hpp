#pragma once

#include <memory>

namespace picotorrent
{
namespace core
{
    class session;
}
namespace ui
{
    class main_window;
}
namespace app
{
namespace controllers
{
    class add_torrent_controller
    {
    public:
        add_torrent_controller(const std::shared_ptr<core::session> &sess, const std::shared_ptr<ui::main_window> &wnd_ptr);
        void execute();

    private:
        std::shared_ptr<core::session> sess_;
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
