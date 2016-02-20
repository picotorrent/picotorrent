#pragma once

#include <memory>
#include <vector>

namespace picotorrent
{
namespace core
{
    class session;
    class torrent_info;
}
namespace client
{
namespace ui
{
namespace dialogs
{
    class magnet_link_dialog;
}
    class main_window;
}
namespace controllers
{
    class add_magnet_link_controller
    {
    public:
        add_magnet_link_controller(const std::shared_ptr<core::session> &sess, const std::shared_ptr<ui::main_window> &wnd);
        void execute();

    protected:
        void on_add_links();
        void on_metadata(const std::shared_ptr<core::torrent_info> &ti);

    private:
        std::shared_ptr<core::session> sess_;
        std::shared_ptr<ui::dialogs::magnet_link_dialog> magnet_dlg_;
        std::shared_ptr<ui::main_window> wnd_;

        int waiting_for_;
        std::vector<std::shared_ptr<core::torrent_info>> metadata_;
    };
}
}
}
