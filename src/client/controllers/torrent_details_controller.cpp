#include <picotorrent/client/controllers/torrent_details_controller.hpp>

#include <picotorrent/core/peer.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/torrent_info.hpp>
#include <picotorrent/core/tracker.hpp>
#include <picotorrent/client/ui/dialogs/add_tracker_dialog.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/ui/property_sheets/details/files_page.hpp>
#include <picotorrent/client/ui/property_sheets/details/overview_page.hpp>
#include <picotorrent/client/ui/property_sheets/details/peers_page.hpp>
#include <picotorrent/client/ui/property_sheets/details/trackers_page.hpp>

#include <prsht.h>
#include <strsafe.h>

namespace details = picotorrent::client::ui::property_sheets::details;
using picotorrent::client::controllers::torrent_details_controller;
using picotorrent::client::ui::dialogs::add_tracker_dialog;
using picotorrent::client::ui::main_window;
using picotorrent::core::peer;
using picotorrent::core::torrent;
using picotorrent::core::torrent_info;
using picotorrent::core::tracker;

torrent_details_controller::torrent_details_controller(
    const std::shared_ptr<main_window> &wnd,
    const std::shared_ptr<torrent> &torrent)
    : torrent_(torrent),
    wnd_(wnd),
    files_(std::make_unique<details::files_page>()),
    overview_(std::make_unique<details::overview_page>()),
    peers_(std::make_unique<details::peers_page>()),
    trackers_(std::make_unique<details::trackers_page>())
{
    files_->on_activate().connect([this]() { set_active_page(torrent_details_controller::files); });
    files_->on_init().connect(std::bind(&torrent_details_controller::on_files_init, this));
    files_->on_set_file_priority().connect(std::bind(&torrent_details_controller::on_files_set_prio, this, std::placeholders::_1));
    overview_->on_activate().connect([this]() { set_active_page(torrent_details_controller::overview); });
    overview_->on_apply().connect(std::bind(&torrent_details_controller::on_overview_apply, this));
    overview_->on_init().connect(std::bind(&torrent_details_controller::on_overview_init, this));

    peers_->on_activate().connect([this]() { set_active_page(torrent_details_controller::peers); });
    
    trackers_->on_activate().connect([this]() { set_active_page(torrent_details_controller::trackers); });
    trackers_->on_add_tracker().connect(std::bind(&torrent_details_controller::on_trackers_add, this));
    trackers_->on_remove_trackers().connect(std::bind(&torrent_details_controller::on_trackers_remove, this, std::placeholders::_1));

    torrent_->on_updated().connect(std::bind(&torrent_details_controller::on_torrent_updated, this));
}

torrent_details_controller::~torrent_details_controller()
{
    torrent_->on_updated().disconnect(std::bind(&torrent_details_controller::on_torrent_updated, this));
}

void torrent_details_controller::execute()
{
    PROPSHEETPAGE p[] =
    {
        *overview_,
        *files_,
        *peers_,
        *trackers_
    };

    TCHAR t[1024];
    StringCchPrintf(t, ARRAYSIZE(t), L"%s", to_wstring(torrent_->name()).c_str());

    PROPSHEETHEADER header = { 0 };
    header.dwSize = sizeof(PROPSHEETHEADER);
    header.dwFlags = PSH_NOCONTEXTHELP | PSH_PROPSHEETPAGE;
    header.hwndParent = wnd_->handle();
    header.hInstance = GetModuleHandle(NULL);
    header.pszCaption = t;
    header.nPages = ARRAYSIZE(p);
    header.nStartPage = 0;
    header.ppsp = (LPCPROPSHEETPAGE)p;
    header.pfnCallback = NULL;

    PropertySheet(&header);
}

void torrent_details_controller::on_files_init()
{
    std::shared_ptr<const torrent_info> ti = torrent_->torrent_info();

    if (!ti)
    {
        // TODO: Disable files list
        return;
    }

    std::vector<int64_t> progress;
    torrent_->file_progress(progress);
    std::vector<int> priorities = torrent_->file_priorities();

    // Loop through each file in torrent and add it
    for (int i = 0; i < ti->num_files(); i++)
    {
        float p = (float)progress[i] / ti->file_size(i);

        files_->add_file(
            ti->file_path(i),
            ti->file_size(i),
            p,
            priorities[i]);
    }
}

void torrent_details_controller::on_files_set_prio(const std::pair<int, int> &p)
{
    torrent_->file_priority(p.first, p.second);
}

void torrent_details_controller::on_overview_apply()
{
    int dl_limit = overview_->dl_limit();
    if (dl_limit > 0) { dl_limit *= 1024; }

    int ul_limit = overview_->ul_limit();
    if (ul_limit > 0) { ul_limit *= 1024; }

    torrent_->set_download_limit(dl_limit);
    torrent_->set_upload_limit(ul_limit);
    torrent_->set_max_connections(overview_->max_connections());
    torrent_->set_max_uploads(overview_->max_uploads());
    torrent_->set_sequential_download(overview_->sequential_download());
}

void torrent_details_controller::on_overview_init()
{
    int dl_limit = torrent_->download_limit();
    int ul_limit = torrent_->upload_limit();
    int max_connections = torrent_->max_connections();
    int max_uploads = torrent_->max_uploads();

    // Adjust values
    if (dl_limit < 0) { dl_limit = 0; }
    if (dl_limit > 0) { dl_limit /= 1024; }
    if (ul_limit < 0) { ul_limit = 0; }
    if (ul_limit > 0) { ul_limit /= 1024; }
    if (max_connections >= 16777215) { max_connections = -1; }
    if (max_uploads >= 16777215) { max_uploads = -1; }

    overview_->dl_limit(dl_limit);
    overview_->ul_limit(ul_limit);
    overview_->max_connections(max_connections);
    overview_->max_uploads(max_uploads);
    overview_->sequential_download(torrent_->sequential_download());
}

void torrent_details_controller::on_torrent_updated()
{
    switch (active_page_)
    {
    case torrent_details_controller::files:
        update_files();
        break;
    case torrent_details_controller::peers:
        update_peers();
        break;
    case torrent_details_controller::trackers:
        update_trackers();
        break;
    }
}

void torrent_details_controller::on_trackers_add()
{
    add_tracker_dialog add_tracker;
    switch (add_tracker.show_modal(trackers_->handle()))
    {
    case IDOK:
    {
        std::wstring url = add_tracker.get_url();
        torrent_->add_tracker(to_string(url));
        update_trackers();
        break;
    }
    }
}

void torrent_details_controller::on_trackers_remove(const std::vector<std::string> &trackers)
{
    torrent_->remove_trackers(trackers);
    update_trackers();
}

void torrent_details_controller::set_active_page(torrent_details_controller::active_page_t page)
{
    active_page_ = page;
    on_torrent_updated();
}

void torrent_details_controller::update_files()
{
    std::shared_ptr<const torrent_info> ti = torrent_->torrent_info();
    if (!ti) { return; }

    std::vector<int64_t> progress;
    torrent_->file_progress(progress);

    for (int i = 0; i < ti->num_files(); i++)
    {
        float p = (float)progress[i] / ti->file_size(i);
        files_->update_file_progress(i, p);
    }

    files_->refresh();
}

void torrent_details_controller::update_peers()
{
    peers_->refresh(torrent_->get_peers());
}

void torrent_details_controller::update_trackers()
{
    trackers_->refresh(torrent_);
}
