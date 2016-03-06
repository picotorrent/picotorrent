#include <picotorrent/client/controllers/add_magnet_link_controller.hpp>

#include <picotorrent/core/logging/log.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/string_operations.hpp>
#include <picotorrent/core/torrent_info.hpp>
#include <picotorrent/client/controllers/add_torrent_controller.hpp>
#include <picotorrent/client/ui/dialogs/magnet_link_dialog.hpp>
#include <picotorrent/client/ui/main_window.hpp>

#include <regex>

using picotorrent::client::controllers::add_magnet_link_controller;
using picotorrent::core::session;
using picotorrent::core::to_string;
using picotorrent::core::torrent_info;
using picotorrent::client::ui::dialogs::magnet_link_dialog;
using picotorrent::client::ui::main_window;

add_magnet_link_controller::add_magnet_link_controller(
    const std::shared_ptr<session> &sess,
    const std::shared_ptr<main_window> &wnd)
    : sess_(sess),
    wnd_(wnd),
    magnet_dlg_(std::make_shared<magnet_link_dialog>()),
    waiting_for_(0)
{
    magnet_dlg_->on_add_links().connect(std::bind(&add_magnet_link_controller::on_add_links, this));
}

void add_magnet_link_controller::execute()
{
    magnet_dlg_->show_modal(wnd_->handle());
    sess_->on_metadata_received().disconnect(
        std::bind(&add_magnet_link_controller::on_metadata, this, std::placeholders::_1));

    // We got all metadata we wanted (not the case when cancelling, probably :)
    if (waiting_for_ == metadata_.size())
    {
        controllers::add_torrent_controller add(sess_, wnd_);
        add.execute(metadata_);
    }
}

void add_magnet_link_controller::on_add_links()
{
    sess_->on_metadata_received().connect(
        std::bind(&add_magnet_link_controller::on_metadata, this, std::placeholders::_1));

    std::vector<std::wstring> links = magnet_dlg_->get_links();
    std::wstring magnet_prefix = L"magnet:?xt=urn:btih:";

    for (std::wstring link : links)
    {
        link.erase(link.find_last_not_of(L"\r") + 1);
        if (link.empty()) { continue; }

        // If only info hash, append magnet link template
        if ((link.size() == 40 && !std::regex_match(link, std::wregex(L"[^0-9A-Fa-f]")))
            || (link.size() == 32 && !std::regex_match(link, std::wregex(L""))))
        {
            link = magnet_prefix + link;
        }

        // Check if link starts with "magnet:?xt=urn:btih:"
        if (link.substr(0, magnet_prefix.size()) != magnet_prefix)
        {
            continue;
        }

        sess_->get_metadata(to_string(link));
        waiting_for_ += 1;
    }

    if (waiting_for_ == 0)
    {
        // Show alert.
        return;
    }

    magnet_dlg_->disable_actions();
    magnet_dlg_->start_progress();
    magnet_dlg_->update_status_text(0, waiting_for_);
}

void add_magnet_link_controller::on_metadata(const std::shared_ptr<torrent_info> &ti)
{
    LOG(debug) << "Found metadata for " << ti->name();
    metadata_.push_back(ti);
    magnet_dlg_->update_status_text((int)metadata_.size(), waiting_for_);

    if (metadata_.size() == waiting_for_)
    {
        magnet_dlg_->close();
    }
}
