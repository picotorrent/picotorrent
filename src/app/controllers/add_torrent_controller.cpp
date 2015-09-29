#include <picotorrent/app/controllers/add_torrent_controller.hpp>

#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/env.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent_file.hpp>
#include <picotorrent/filesystem/file.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/logging/log.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/open_torrent_dialog.hpp>

namespace core = picotorrent::core;
namespace fs = picotorrent::filesystem;
namespace ui = picotorrent::ui;
using picotorrent::app::controllers::add_torrent_controller;

add_torrent_controller::add_torrent_controller(
    const std::shared_ptr<core::session> &sess,
    const std::shared_ptr<ui::main_window> &wnd_ptr)
    : sess_(sess),
    wnd_(wnd_ptr)
{
}

void add_torrent_controller::execute()
{
    ui::open_torrent_dialog dlg;
    dlg.show(wnd_->handle());

    for (fs::path &p : dlg.get_paths())
    {
        fs::file f(p);
        std::vector<char> buf;

        try
        {
            f.read_all(buf);
        }
        catch (const std::exception &e)
        {
            LOG(error) << "Error when reading file: " << e.what();
            continue;
        }

        core::torrent_file_ptr torrent = std::make_shared<core::torrent_file>(buf);

        core::add_request req;
        req.set_save_path(core::env::get_user_downloads_directory().to_string());
        req.set_torrent_file(torrent);

        sess_->add_torrent(req);
    }
}
