#include <picotorrent/app/controllers/move_torrent_controller.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/open_file_dialog.hpp>

namespace fs = picotorrent::filesystem;
using namespace picotorrent::common;
using picotorrent::app::controllers::move_torrent_controller;
using picotorrent::core::torrent;
using picotorrent::ui::main_window;

move_torrent_controller::move_torrent_controller(
    const std::shared_ptr<main_window> &wnd,
    const std::shared_ptr<torrent> &torrent)
    : torrent_(torrent),
    wnd_(wnd)
{
}

move_torrent_controller::~move_torrent_controller()
{
}

void move_torrent_controller::execute()
{
    std::wstring s = to_wstring(torrent_->save_path());

    ui::open_file_dialog dlg;
    dlg.set_folder(s);
    dlg.set_options(dlg.options() | FOS_PICKFOLDERS);
    dlg.set_title(TEXT("Select destination"));

    dlg.show(wnd_->handle());

    std::vector<fs::path> paths = dlg.get_paths();

    if (paths.size() > 0)
    {
        torrent_->move_storage(to_string(paths[0].to_string()));
    }
}
