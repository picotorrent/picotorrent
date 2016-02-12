#include <picotorrent/client/controllers/application_close_controller.hpp>

#include <picotorrent/core/configuration.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/task_dialog.hpp>

#define ID_DLG_CLOSE    1000
#define ID_DLG_MINIMIZE 1001

using picotorrent::client::controllers::application_close_controller;
using picotorrent::client::ui::main_window;
using picotorrent::client::ui::task_dialog;
using picotorrent::core::configuration;

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
    task_dialog dlg;
    dlg.add_button(ID_DLG_CLOSE, TR("close_picotorrent"),
        [this]()
    {
        return false;
    });

    dlg.add_button(ID_DLG_MINIMIZE, TR("minimize_to_tray"),
        [this]()
    {
        return false;
    });

    dlg.set_common_buttons(TDCBF_CANCEL_BUTTON);
    dlg.set_content(TR("confirm_close_description"));
    dlg.set_main_icon(TD_INFORMATION_ICON);
    dlg.set_main_instruction(TR("confirm_close"));
    dlg.set_parent(wnd_->handle());
    dlg.set_title(L"PicoTorrent");
    dlg.set_verification_text(TR("remember_choice"));

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
