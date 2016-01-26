#include <picotorrent/app/controllers/torrent_details_controller.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/ui/main_window.hpp>

#include <picotorrent/ui/property_sheets/details/files_page.hpp>
#include <picotorrent/ui/property_sheets/details/overview_page.hpp>

#include <prsht.h>
#include <strsafe.h>

namespace details = picotorrent::ui::property_sheets::details;
using picotorrent::app::controllers::torrent_details_controller;
using picotorrent::common::to_wstring;
using picotorrent::core::torrent;
using picotorrent::ui::main_window;

torrent_details_controller::torrent_details_controller(
    const std::shared_ptr<main_window> &wnd,
    const std::shared_ptr<torrent> &torrent)
    : torrent_(torrent),
    wnd_(wnd),
    files_(std::make_unique<details::files_page>()),
    overview_(std::make_unique<details::overview_page>())
{
    files_->set_init_callback(std::bind(&torrent_details_controller::on_files_init, this));

    overview_->set_apply_callback(std::bind(&torrent_details_controller::on_overview_apply, this));
    overview_->set_init_callback(std::bind(&torrent_details_controller::on_overview_init, this));
}

torrent_details_controller::~torrent_details_controller()
{
}

void torrent_details_controller::execute()
{
    PROPSHEETPAGE p[] =
    {
        *overview_,
        *files_
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
