#include <picotorrent/client/controllers/add_torrent_controller.hpp>

#include <algorithm>
#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/pal.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/torrent_info.hpp>
#include <picotorrent/client/command_line.hpp>
#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/logging/log.hpp>
#include <picotorrent/client/ui/dialogs/add_torrent_dialog.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/open_file_dialog.hpp>
#include <picotorrent/client/ui/open_torrent_dialog.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/task_dialog.hpp>

#include <sstream>

#include <windows.h>
#include <shobjidl.h>
#include <shlwapi.h>

const GUID DLG_OPEN = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x60 } };
const GUID DLG_SAVE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x61 } };

namespace core = picotorrent::core;
namespace ui = picotorrent::client::ui;
using picotorrent::client::command_line;
using picotorrent::client::to_wstring;
using picotorrent::client::controllers::add_torrent_controller;
using picotorrent::client::ui::dialogs::add_torrent_dialog;
using picotorrent::client::configuration;
using picotorrent::core::pal;

add_torrent_controller::add_torrent_controller(
    const std::shared_ptr<core::session> &sess,
    const std::shared_ptr<ui::main_window> &wnd_ptr)
    : sess_(sess),
    dlg_(std::make_shared<add_torrent_dialog>()),
    wnd_(wnd_ptr)
{
    dlg_->on_update_storage_mode().connect(std::bind(&add_torrent_controller::on_update_storage_mode, this));
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
    std::string default_save_path = cfg.default_save_path();

    for (const std::string &magnet : cmd.magnet_links())
    {
        auto req = std::make_shared<core::add_request>();
        req->set_save_path(default_save_path);
        req->set_url(magnet);

        requests_.push_back(req);
    }

    show_add_dialog();
}

void add_torrent_controller::execute(const std::vector<std::string> &files)
{
    if (files.empty())
    {
        return;
    }

    add_files(files);
    show_add_dialog();
}

void add_torrent_controller::execute(const std::vector<std::shared_ptr<core::torrent_info>> &torrents)
{
    if (torrents.empty())
    {
        return;
    }

    configuration &cfg = configuration::instance();
    std::string default_save_path = cfg.default_save_path();

    for (const std::shared_ptr<core::torrent_info> &ti : torrents)
    {
        auto req = std::make_shared<core::add_request>();
        req->set_save_path(default_save_path);
        req->set_torrent_info(ti);

        requests_.push_back(req);
    }

    show_add_dialog();
}

void add_torrent_controller::add_files(const std::vector<std::string> &paths)
{
    configuration &cfg = configuration::instance();
    std::string save_path = cfg.default_save_path();

    for (const std::string &p : paths)
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
        std::vector<std::shared_ptr<core::torrent_info>> existing;

		for (auto &req : requests_)
		{
            if (req->ti() && sess_->has_torrent(req->ti()->info_hash()))
            {
                existing.push_back(req->ti());
                continue;
            }

			sess_->add_torrent(req);
		}

        if (existing.size() > 0)
        {
            std::stringstream ss;

            for (auto &ti : existing)
            {
                ss << ti->name() << std::endl;
            }

            ui::task_dialog dlg;
            dlg.set_common_buttons(TDCBF_OK_BUTTON);
            dlg.set_content(ss.str());
            dlg.set_main_icon(TD_WARNING_ICON);
            dlg.set_main_instruction(TR("torrent_s_already_in_session"));
            dlg.set_title("PicoTorrent");
            dlg.show();
        }
	}
}

void add_torrent_controller::on_dialog_init()
{
    for (auto &req : requests_)
    {
        std::string name = TR("unknown_name");

        if (req->ti())
        {
            name = req->ti()->name();
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
    std::string sp = get_save_path();

    if (sp.empty())
    {
        return;
    }

    auto &req = requests_[dlg_->get_selected_torrent()];
    dlg_->set_save_path(sp);
    req->set_save_path(sp);
}

void add_torrent_controller::on_update_storage_mode()
{
    bool use_full_allocation = dlg_->use_full_allocation();
    auto &req = requests_[dlg_->get_selected_torrent()];

    if (use_full_allocation)
    {
        req->set_allocation_mode(core::add_request::allocation_mode_t::full);
    }
    else
    {
        req->set_allocation_mode(core::add_request::allocation_mode_t::sparse);
    }
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

    HMENU prioMenu = CreateMenu();
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_MAX, to_wstring(TR("maximum")).c_str());
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_HIGH, to_wstring(TR("high")).c_str());
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_NORMAL, to_wstring(TR("normal")).c_str());
    AppendMenu(prioMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_SKIP, to_wstring(TR("do_not_download")).c_str());

    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_POPUP, (UINT_PTR)prioMenu, to_wstring(TR("priority")).c_str());

    auto &req = requests_[dlg_->get_selected_torrent()];

    // If only one file is selected, check that files priority
    if (files.size() == 1)
    {
        int prio = req->file_priority(files[0]);

        switch (prio)
        {
        case core::torrent::do_not_download:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_SKIP, MF_BYCOMMAND | MF_CHECKED);
            break;
        case core::torrent::normal:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_NORMAL, MF_BYCOMMAND | MF_CHECKED);
            break;
        case core::torrent::high:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_HIGH, MF_BYCOMMAND | MF_CHECKED);
            break;
        case core::torrent::maximum:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_MAX, MF_BYCOMMAND | MF_CHECKED);
            break;
        }
    }

    POINT p;
    GetCursorPos(&p);

    int res = TrackPopupMenu(
        menu,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        wnd_->handle(),
        NULL);

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
    core::torrent_info_ptr ti = req->ti();

    if (ti)
    {
        dlg_->set_size(ti->total_size());
        dlg_->clear_torrent_files();
        dlg_->enable_files();

        for (int i = 0; i < ti->num_files(); i++)
        {
            dlg_->add_torrent_file(
                ti->file_path(i),
                ti->file_size(i),
                get_prio_str(req->file_priority(i)));
        }
    }
    else
    {
        dlg_->clear_torrent_files();
        dlg_->disable_files();
        dlg_->set_size(TR("unknown_size"));
    }

    dlg_->set_save_path(req->save_path());
}

std::string add_torrent_controller::get_save_path()
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
    dlg.set_title(TR("choose_save_path"));

    dlg.show(wnd_->handle());

    std::vector<std::string> paths = dlg.get_paths();

    if (paths.size() > 0)
    {
        return paths[0];
    }

    return "";
}

std::string add_torrent_controller::get_prio_str(int prio)
{
    switch (prio)
    {
    case core::torrent::do_not_download:
        return TR("do_not_download");
    case core::torrent::normal:
        return TR("normal");
    case core::torrent::high:
        return TR("high");
    case core::torrent::maximum:
        return TR("maximum");
    default:
        return TR("unknown");
    }
}
