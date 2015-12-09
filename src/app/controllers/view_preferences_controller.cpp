#include <picotorrent/app/controllers/view_preferences_controller.hpp>

#include <picotorrent/config/configuration.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/dialogs/preferences_dialog.hpp>
#include <picotorrent/ui/resources.hpp>

namespace ui = picotorrent::ui;
using picotorrent::config::configuration;
using picotorrent::app::controllers::view_preferences_controller;
using picotorrent::ui::dialogs::preferences_dialog;

view_preferences_controller::view_preferences_controller(const std::shared_ptr<ui::main_window> &wnd)
    : wnd_(wnd)
{
}

void view_preferences_controller::execute()
{
    preferences_dialog dlg;

    configuration &cfg = configuration::instance();
    dlg.set_default_save_path(cfg.default_save_path());
    dlg.set_listen_port(cfg.listen_port());
    dlg.set_prompt_for_save_path(cfg.prompt_for_save_path());

    switch(dlg.show_modal(wnd_->handle()))
    {
    case IDOK:
    {
        cfg.set_default_save_path(dlg.default_save_path());
        cfg.set_listen_port(dlg.listen_port());
        cfg.set_prompt_for_save_path(dlg.prompt_for_save_path());
        break;
    }
    }
}
