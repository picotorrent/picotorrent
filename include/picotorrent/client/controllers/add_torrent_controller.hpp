#pragma once

#include <memory>
#include <string>
#include <vector>

namespace picotorrent
{
namespace common
{
    class command_line;
}
namespace core
{
    class add_request;
    class session;
    class torrent_info;
}
namespace client
{
namespace ui
{
namespace dialogs
{
    class add_torrent_dialog;
}
    class main_window;
}

namespace controllers
{
    class add_torrent_controller
    {
    public:
        add_torrent_controller(const std::shared_ptr<core::session> &sess, const std::shared_ptr<ui::main_window> &wnd_ptr);

        void execute();
        void execute(const common::command_line &cmd);
        void execute(const std::vector<std::string> &files);
        void execute(const std::vector<std::shared_ptr<core::torrent_info>> &torrents);

    protected:
        void on_dialog_init();
        void on_edit_save_path();
        void on_update_storage_mode();
        void on_torrent_change(int index);
        void on_torrent_files_context_menu(const std::vector<int> &files);
        void show_torrent(int index);

    private:
        void add_files(const std::vector<std::string> &paths);
        void show_add_dialog();

        std::string get_save_path();
        std::string get_prio_str(int prio);

        std::shared_ptr<core::session> sess_;
        std::shared_ptr<ui::dialogs::add_torrent_dialog> dlg_;
        std::shared_ptr<ui::main_window> wnd_;
        std::vector<std::shared_ptr<core::add_request>> requests_;
    };
}
}
}
