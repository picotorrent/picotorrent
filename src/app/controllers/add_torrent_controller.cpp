#include <picotorrent/app/controllers/add_torrent_controller.hpp>

#include <algorithm>
#include <picotorrent/app/command_line.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/config/configuration.hpp>
#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>
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
    dlg_->set_edit_save_path_callback(std::bind(&add_torrent_controller::on_edit_save_path, this));
    dlg_->set_file_context_menu_callback(std::bind(&add_torrent_controller::on_torrent_files_context_menu, this, std::placeholders::_1));
}

void add_torrent_controller::execute()
{
    ui::open_torrent_dialog dlg;
    dlg.set_guid(DLG_OPEN);
	dlg.set_options(dlg.options() | FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST);
    dlg.show(wnd_->handle());

    add_files(dlg.get_paths());
    show_add_dialog();
}

void add_torrent_controller::execute(const command_line &cmd)
{
    if (cmd.files().empty() && cmd.magnet_links().empty())
    {
        return;
    }

    add_files(cmd.files());

    configuration &cfg = configuration::instance();
    std::wstring default_save_path = cfg.default_save_path();

    for (const std::wstring &magnet : cmd.magnet_links())
    {
        auto req = std::make_shared<core::add_request>();
        req->set_save_path(default_save_path);
        req->set_url(magnet);

        requests_.push_back(req);
    }

    show_add_dialog();
}

void add_torrent_controller::add_files(const std::vector<fs::path> &paths)
{
    configuration &cfg = configuration::instance();
    std::wstring save_path = cfg.default_save_path();

    for (const fs::path &p : paths)
    {
        auto ti = core::torrent_info::try_load(p);
        if (!ti) { continue; }

        auto r = std::make_shared<core::add_request>();
        r->set_torrent_info(ti);
        r->set_save_path(save_path);

        requests_.push_back(r);
    }
}

void add_torrent_controller::show_add_dialog()
{
    if (requests_.empty())
    {
        return;
    }

	configuration &cfg = configuration::instance();
	int res = IDOK;

	if (cfg.prompt_for_save_path())
	{
		res = dlg_->show_modal(wnd_->handle());
	}

	if (res == IDOK)
	{
		for (auto &req : requests_)
		{
			sess_->add_torrent(req);
		}
	}
}

void add_torrent_controller::on_dialog_init()
{
    for (auto &req : requests_)
    {
        std::wstring name = L"Unknown name";

        if (req->torrent_info())
        {
            name = to_wstring(req->torrent_info()->name());
        }
        else if(!req->name().empty())
        {
            name = req->name();
        }

        dlg_->add_torrent(name);
    }

    dlg_->set_selected_item(0);
    show_torrent(0);
}

void add_torrent_controller::on_edit_save_path()
{
    std::wstring sp = get_save_path();

    if (sp.empty())
    {
        return;
    }

    auto &req = requests_[dlg_->get_selected_torrent()];
    dlg_->set_save_path(sp);
    req->set_save_path(sp);
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
        case core::torrent::do_not_download:
            sub.check_item(TORRENT_FILE_PRIO_SKIP);
            break;
        case core::torrent::normal:
            sub.check_item(TORRENT_FILE_PRIO_NORMAL);
            break;
        case core::torrent::high:
            sub.check_item(TORRENT_FILE_PRIO_HIGH);
            break;
        case core::torrent::maximum:
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
            req->set_file_priority(i, core::torrent::do_not_download);
            dlg_->set_file_priority(i, get_prio_str(core::torrent::do_not_download));
            break;
        case TORRENT_FILE_PRIO_NORMAL:
            req->set_file_priority(i, core::torrent::normal);
            dlg_->set_file_priority(i, get_prio_str(core::torrent::normal));
            break;
        case TORRENT_FILE_PRIO_HIGH:
            req->set_file_priority(i, core::torrent::high);
            dlg_->set_file_priority(i, get_prio_str(core::torrent::high));
            break;
        case TORRENT_FILE_PRIO_MAX:
            req->set_file_priority(i, core::torrent::maximum);
            dlg_->set_file_priority(i, get_prio_str(core::torrent::maximum));
            break;
        }
    }
}

void add_torrent_controller::show_torrent(int index)
{
    std::shared_ptr<core::add_request> &req = requests_[index];
    core::torrent_info_ptr ti = req->torrent_info();

    if (ti)
    {
        std::wstring friendly_size(L"\0", 64);
        StrFormatByteSize64((UINT)ti->total_size(), &friendly_size[0], (UINT)friendly_size.size());
        dlg_->set_size(friendly_size);

        dlg_->clear_torrent_files();
        dlg_->enable_files();

        for (int i = 0; i < ti->num_files(); i++)
        {
            std::wstring file_size(L"\0", 64);
            StrFormatByteSize64((UINT)ti->file_size(i), &file_size[0], (UINT)file_size.size());

            dlg_->add_torrent_file(
                to_wstring(ti->file_path(i)),
                file_size,
                get_prio_str(req->file_priority(i)));
        }
    }
    else
    {
        dlg_->clear_torrent_files();
        dlg_->disable_files();
        dlg_->set_size(L"Unknown size");
    }

    dlg_->set_save_path(req->save_path());
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
    case core::torrent::do_not_download:
        return L"Do not download";
    case core::torrent::normal:
        return L"Normal";
    case core::torrent::high:
        return L"High";
    case core::torrent::maximum:
        return L"Maximum";
    default:
        return L"Unknown priority";
    }
}
