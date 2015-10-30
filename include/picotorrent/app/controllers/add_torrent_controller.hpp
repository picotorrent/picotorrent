#pragma once

#include <memory>
#include <string>
#include <vector>

namespace picotorrent
{
namespace core
{
    class session;
}
namespace filesystem
{
    class path;
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
        void execute(const std::vector<filesystem::path> &files);

    private:
        std::shared_ptr<core::session> sess_;
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
