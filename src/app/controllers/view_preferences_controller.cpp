#include <picotorrent/app/controllers/view_preferences_controller.hpp>

#include <picotorrent/config/configuration.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/preferences_dialog.hpp>
#include <picotorrent/ui/resources.hpp>

namespace ui = picotorrent::ui;
using picotorrent::config::configuration;
using picotorrent::app::controllers::view_preferences_controller;
using picotorrent::ui::preferences_dialog;

view_preferences_controller::view_preferences_controller(const std::shared_ptr<ui::main_window> &wnd)
    : wnd_(wnd)
{
}

void view_preferences_controller::execute()
{
    preferences_dialog dlg(wnd_->handle());
    dlg.on_init(std::bind(&view_preferences_controller::init_dlg, this, std::placeholders::_1));
    dlg.on_ok(std::bind(&view_preferences_controller::ok_dlg, this, std::placeholders::_1));

    dlg.do_modal();
}

void view_preferences_controller::init_dlg(preferences_dialog &dlg)
{
    configuration &cfg = configuration::instance();

    dlg.set_text(ID_PREFS_DEFSAVEPATH, cfg.default_save_path());
    dlg.set_text(ID_PREFS_LISTENPORT, std::to_wstring(cfg.listen_port()));
    dlg.set_checked(ID_PREFS_PROMPTFORSAVEPATH, cfg.prompt_for_save_path());
}

void view_preferences_controller::ok_dlg(preferences_dialog &dlg)
{
    configuration &cfg = configuration::instance();

    std::wstring savePath = dlg.get_text(ID_PREFS_DEFSAVEPATH);
    std::wstring listenPortText = dlg.get_text(ID_PREFS_LISTENPORT);
    int listenPort = std::stoi(listenPortText);
    bool promptForSavePath = dlg.get_checked(ID_PREFS_PROMPTFORSAVEPATH);

    cfg.set_default_save_path(savePath);
    cfg.set_listen_port(listenPort);
    cfg.set_prompt_for_save_path(promptForSavePath);
}
