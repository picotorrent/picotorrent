#include <picotorrent/client/controllers/remove_torrent_controller.hpp>

#include <algorithm>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/task_dialog.hpp>

using picotorrent::client::configuration;
using picotorrent::core::session;
using picotorrent::core::torrent;
using picotorrent::client::controllers::remove_torrent_controller;
using picotorrent::client::ui::main_window;
using picotorrent::client::ui::task_dialog;

remove_torrent_controller::remove_torrent_controller(
    const std::shared_ptr<main_window> &wnd,
    const std::shared_ptr<session> &session,
    const std::vector<std::shared_ptr<torrent>> &torrents)
    : wnd_(wnd),
    session_(session),
    torrents_(torrents)
{
}

remove_torrent_controller::~remove_torrent_controller()
{
}

void remove_torrent_controller::execute(bool remove_files)
{
    if (torrents_.empty())
    {
        return;
    }

    configuration &cfg = configuration::instance();

    if (remove_files && cfg.prompt_for_remove_data())
    {
        task_dialog dlg;
        dlg.set_common_buttons(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON);
        dlg.set_content(TR("confirm_remove_description"));
        dlg.set_main_icon(TD_WARNING_ICON);
        dlg.set_main_instruction(TR("confirm_remove"));
        dlg.set_parent(wnd_->handle());
        dlg.set_title("PicoTorrent");
        dlg.set_verification_text(TR("do_not_ask_again"));

        switch (dlg.show())
        {
        case IDCANCEL:
            return;
        }

        if (dlg.is_verification_checked())
        {
            cfg.set_prompt_for_remove_data(false);
        }
    }

    for (const std::shared_ptr<torrent> &t : torrents_)
    {
        session_->remove_torrent(t, remove_files);
    }
}
