#include <picotorrent/app/controllers/application_close_controller.hpp>

#include <picotorrent/config/configuration.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/task_dialog.hpp>

#define ID_DLG_CLOSE    1000
#define ID_DLG_MINIMIZE 1001

using picotorrent::app::controllers::application_close_controller;
using picotorrent::config::configuration;
using picotorrent::ui::main_window;
using picotorrent::ui::task_dialog;

application_close_controller::application_close_controller(const std::shared_ptr<main_window> &wnd)
    : wnd_(wnd)
{
}

bool application_close_controller::execute()
{
    configuration::close_action_t action = configuration::instance().close_action();

    switch (action)
    {
    case configuration::close_action_t::prompt:
        return prompt_for_exit();
    case configuration::close_action_t::minimize:
        ShowWindow(wnd_->handle(), SW_HIDE);
        return false;
    }

    return true;
}

bool application_close_controller::prompt_for_exit()
{
    ui::task_dialog dlg;
    dlg.add_button(ID_DLG_CLOSE, L"Close PicoTorrent",
        [this]()
    {
        return false;
    });

    dlg.add_button(ID_DLG_MINIMIZE, L"Minimize to tray",
        [this]()
    {
        return false;
    });

    dlg.set_common_buttons(TDCBF_CANCEL_BUTTON);
    dlg.set_content(L"Are you sure you want to close PicoTorrent?");
    dlg.set_main_icon(TD_INFORMATION_ICON);
    dlg.set_main_instruction(L"Confirm close");
    dlg.set_parent(wnd_->handle());
    dlg.set_title(L"PicoTorrent");
    dlg.set_verification_text(L"Remember my choice");

    configuration &cfg = configuration::instance();
    int res = dlg.show();

    if (res == IDCANCEL)
    {
        return false;
    }

    if (dlg.is_verification_checked())
    {
        switch (res)
        {
        case ID_DLG_CLOSE:
            cfg.set_close_action(configuration::close_action_t::close);
            break;
        case ID_DLG_MINIMIZE:
            cfg.set_close_action(configuration::close_action_t::minimize);
            break;
        }
    }

    if (res == ID_DLG_MINIMIZE)
    {
        ShowWindow(wnd_->handle(), SW_HIDE);
        return false;
    }

    return true;
}
