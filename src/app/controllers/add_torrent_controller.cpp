#include <picotorrent/app/controllers/add_torrent_controller.hpp>

#include <picotorrent/app/command_line.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/config/configuration.hpp>
#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent_file.hpp>
#include <picotorrent/filesystem/file.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/logging/log.hpp>
#include <picotorrent/ui/dialogs/add_torrent_dialog.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/open_file_dialog.hpp>
#include <picotorrent/ui/open_torrent_dialog.hpp>

#include <windows.h>
#include <shobjidl.h>
#include <shlwapi.h>

const GUID DLG_OPEN = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x60 } };
const GUID DLG_SAVE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x61 } };

namespace core = picotorrent::core;
namespace fs = picotorrent::filesystem;
namespace ui = picotorrent::ui;
using picotorrent::app::command_line;
using picotorrent::app::controllers::add_torrent_controller;
using picotorrent::common::to_wstring;
using picotorrent::config::configuration;
using picotorrent::ui::dialogs::add_torrent_dialog;

add_torrent_controller::add_torrent_controller(
    const std::shared_ptr<core::session> &sess,
    const std::shared_ptr<ui::main_window> &wnd_ptr)
    : sess_(sess),
    dlg_(std::make_shared<add_torrent_dialog>()),
    wnd_(wnd_ptr)
{
    dlg_->set_init_callback(std::bind(&add_torrent_controller::on_dialog_init, this));
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

    configuration &cfg = configuration::instance();
    std::wstring save_path = cfg.default_save_path();

    for (fs::path &p : dlg.get_paths())
    {
        auto r = std::make_shared<core::add_request>();
        r->set_torrent_file(get_torrent_file(p));
        r->set_save_path(save_path);

        requests_.push_back(r);
    }

    dlg_->show_modal(wnd_->handle());
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

void add_torrent_controller::on_dialog_init()
{
    for (auto &req : requests_)
    {
        dlg_->add_torrent(to_wstring(req->torrent_file()->name()));
    }

    dlg_->set_selected_item(0);
    show_torrent(0);
}

void add_torrent_controller::show_torrent(int index)
{
    std::shared_ptr<core::add_request> &req = requests_[index];

    std::wstring friendly_size(L"\0", 64);
    StrFormatByteSize64(req->torrent_file()->total_size(), &friendly_size[0], friendly_size.size());

    dlg_->set_save_path(req->save_path());
    dlg_->set_size(friendly_size);
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

core::torrent_file_ptr add_torrent_controller::get_torrent_file(const fs::path &path)
{
    if (!path.exists())
    {
        return nullptr;
    }

    fs::file f(path);
    std::vector<char> buf;

    try
    {
        f.read_all(buf);
    }
    catch (const std::exception &e)
    {
        LOG(error) << "Error when reading file: " << e.what();
        return nullptr;
    }

    return std::make_shared<core::torrent_file>(buf);
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
