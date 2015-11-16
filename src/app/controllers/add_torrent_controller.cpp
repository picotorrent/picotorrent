#include <picotorrent/app/controllers/add_torrent_controller.hpp>

#include <picotorrent/app/command_line.hpp>
#include <picotorrent/config/configuration.hpp>
#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent_file.hpp>
#include <picotorrent/filesystem/file.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/logging/log.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/open_file_dialog.hpp>
#include <picotorrent/ui/open_torrent_dialog.hpp>

#include <windows.h>
#include <shobjidl.h>

const GUID DLG_OPEN = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x60 } };
const GUID DLG_SAVE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x61 } };

namespace core = picotorrent::core;
namespace fs = picotorrent::filesystem;
namespace ui = picotorrent::ui;
using picotorrent::app::command_line;
using picotorrent::app::controllers::add_torrent_controller;
using picotorrent::config::configuration;

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
    dlg.set_guid(DLG_OPEN);
    dlg.show(wnd_->handle());

    if (dlg.get_paths().empty())
    {
        return;
    }

    add_files(dlg.get_paths(), get_save_path());
}

void add_torrent_controller::execute(const command_line &cmd)
{
    if (cmd.files().empty() && cmd.magnet_links().empty())
    {
        return;
    }

    std::wstring sp = get_save_path();
    add_files(cmd.files(), sp);

    for (const std::wstring &magnet : cmd.magnet_links())
    {
        core::add_request req;
        req.set_save_path(sp);
        req.set_url(magnet);

        sess_->add_torrent(req);
    }
}

void add_torrent_controller::add_files(const std::vector<fs::path> &files, const std::wstring &save_path)
{
    for (const fs::path &p : files)
    {
        if (!p.exists())
        {
            continue;
        }

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
        req.set_save_path(save_path);
        req.set_torrent_file(torrent);

        sess_->add_torrent(req);
    }
}

std::wstring add_torrent_controller::get_save_path()
{
    configuration &cfg = configuration::instance();

    if (!cfg.prompt_for_save_path())
    {
        return cfg.default_save_path();
    }

    ui::open_file_dialog dlg;
    dlg.set_guid(DLG_SAVE);
    dlg.set_folder(cfg.default_save_path());
    dlg.set_options(dlg.options() | FOS_PICKFOLDERS);
    dlg.set_title(TEXT("Choose save path"));

    dlg.show(wnd_->handle());

    std::vector<fs::path> paths = dlg.get_paths();

    if (paths.size() > 0)
    {
        return paths[0].to_string();
    }

    return L"";
}
