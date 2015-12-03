#pragma once

#include <memory>
#include <string>
#include <vector>

namespace picotorrent
{
namespace core
{
    class add_request;
    class session;
}
namespace filesystem
{
    class path;
}
namespace ui
{
namespace dialogs
{
    class add_torrent_dialog;
}
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

    protected:
        void on_dialog_init();
        void show_torrent(int index);

    private:
        void add_files(const std::vector<filesystem::path> &files, const std::wstring &save_path);

        std::wstring get_save_path();

        std::shared_ptr<core::session> sess_;
        std::shared_ptr<ui::dialogs::add_torrent_dialog> dlg_;
        std::shared_ptr<ui::main_window> wnd_;
        std::vector<std::shared_ptr<core::add_request>> requests_;
    };
}
}
}
