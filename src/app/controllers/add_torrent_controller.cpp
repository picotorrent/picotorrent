#include <picotorrent/app/controllers/add_torrent_controller.hpp>

#include <algorithm>
#include <picotorrent/app/command_line.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/config/configuration.hpp>
#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent_info.hpp>
#include <picotorrent/filesystem/file.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/logging/log.hpp>
#include <picotorrent/ui/controls/menu.hpp>
#include <picotorrent/ui/dialogs/add_torrent_dialog.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/open_file_dialog.hpp>
#include <picotorrent/ui/open_torrent_dialog.hpp>
#include <picotorrent/ui/resources.hpp>

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
    dlg_->set_change_callback(std::bind(&add_torrent_controller::on_torrent_change, this, std::placeholders::_1));
    dlg_->set_file_context_menu_callback(std::bind(&add_torrent_controller::on_torrent_files_context_menu, this, std::placeholders::_1));
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
        auto ti = core::torrent_info::try_load(p);
        if(!ti) { continue; }

        auto r = std::make_shared<core::add_request>();
        r->set_torrent_info(ti);
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
        dlg_->add_torrent(to_wstring(req->torrent_info()->name()));
    }

    dlg_->set_selected_item(0);
    show_torrent(0);
}

void add_torrent_controller::on_torrent_change(int index)
{
    show_torrent(index);
}

void add_torrent_controller::on_torrent_files_context_menu(const std::vector<int> &files)
{
    if (files.empty())
    {
        return;
    }

    ui::controls::menu menu(IDR_TORRENT_FILE_MENU);
    ui::controls::menu sub = menu.get_sub_menu(0);

    auto &req = requests_[dlg_->get_selected_torrent()];

    // If only one file is selected, check that files priority
    if (files.size() == 1)
    {
        int prio = req->file_priority(files[0]);
        
        switch (prio)
        {
        case 0:
            sub.check_item(TORRENT_FILE_PRIO_SKIP);
            break;
        case 1:
            sub.check_item(TORRENT_FILE_PRIO_NORMAL);
            break;
        case 2:
            sub.check_item(TORRENT_FILE_PRIO_HIGH);
            break;
        case 7:
            sub.check_item(TORRENT_FILE_PRIO_MAX);
            break;
        }
    }

    POINT p;
    GetCursorPos(&p);

    int res = sub.show(dlg_->handle(), p);

    for (int i : files)
    {
        switch (res)
        {
        case TORRENT_FILE_PRIO_SKIP:
            req->set_file_priority(i, 0);
            dlg_->set_file_priority(i, get_prio_str(0));
            break;
        case TORRENT_FILE_PRIO_NORMAL:
            req->set_file_priority(i, 1);
            dlg_->set_file_priority(i, get_prio_str(1));
            break;
        case TORRENT_FILE_PRIO_HIGH:
            req->set_file_priority(i, 2);
            dlg_->set_file_priority(i, get_prio_str(2));
            break;
        case TORRENT_FILE_PRIO_MAX:
            req->set_file_priority(i, 7);
            dlg_->set_file_priority(i, get_prio_str(7));
            break;
        }
    }
}

void add_torrent_controller::show_torrent(int index)
{
    std::shared_ptr<core::add_request> &req = requests_[index];

    std::wstring friendly_size(L"\0", 64);
    StrFormatByteSize64(req->torrent_info()->total_size(), &friendly_size[0], friendly_size.size());

    dlg_->set_save_path(req->save_path());
    dlg_->set_size(friendly_size);

    dlg_->clear_torrent_files();

    for (int i = 0; i < req->torrent_info()->num_files(); i++)
    {
        std::wstring file_size(L"\0", 64);
        StrFormatByteSize64(req->torrent_info()->file_size(i), &file_size[0], file_size.size());

        dlg_->add_torrent_file(
            to_wstring(req->torrent_info()->file_name(i)),
            file_size,
            get_prio_str(req->file_priority(i)));
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

        core::torrent_info_ptr torrent = std::make_shared<core::torrent_info>(buf);

        core::add_request req;
        req.set_save_path(save_path);
        req.set_torrent_info(torrent);

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

std::wstring add_torrent_controller::get_prio_str(int prio)
{
    switch (prio)
    {
    case 0:
        return L"Do not download";
    case 1:
        return L"Normal";
    case 2:
        return L"High";
    case 7:
        return L"Maximum";
    default:
        return L"Unknown priority";
    }
}
