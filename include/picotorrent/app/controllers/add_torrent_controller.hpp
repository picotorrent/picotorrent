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
    class command_line;

namespace controllers
{
    class add_torrent_controller
    {
    public:
        add_torrent_controller(const std::shared_ptr<core::session> &sess, const std::shared_ptr<ui::main_window> &wnd_ptr);

        void execute();
        void execute(const command_line &cmd);

    private:
        void add_files(const std::vector<filesystem::path> &files, const std::wstring &save_path);
        std::wstring get_save_path();

        std::shared_ptr<core::session> sess_;
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
