#include <picotorrent/client/controllers/move_torrent_controller.hpp>

#include <picotorrent/core/torrent.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/open_file_dialog.hpp>
#include <picotorrent/common/translator.hpp>

using picotorrent::core::torrent;
using picotorrent::client::controllers::move_torrent_controller;
using picotorrent::client::ui::main_window;

move_torrent_controller::move_torrent_controller(
    const std::shared_ptr<main_window> &wnd,
    const std::vector<std::shared_ptr<torrent>> &torrents)
    : torrents_(torrents),
    wnd_(wnd)
{
}

move_torrent_controller::~move_torrent_controller()
{
}

void move_torrent_controller::execute()
{
    ui::open_file_dialog dlg;

    if (torrents_.size() == 1)
    {
        std::string s = torrents_[0]->save_path();
        dlg.set_folder(s);
    }
    
    dlg.set_options(dlg.options() | FOS_PICKFOLDERS);
    dlg.set_title(TR("select_destination"));

    dlg.show(wnd_->handle());

    std::vector<std::string> paths = dlg.get_paths();

    if (paths.size() > 0)
    {
        for (const std::shared_ptr<torrent> &t : torrents_)
        {
            t->move_storage(paths[0]);
        }
    }
}
