#include <picotorrent/client/controllers/view_preferences_controller.hpp>

#include <picotorrent/core/configuration.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/dialogs/preferences_dialog.hpp>
#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
#include <picotorrent/client/ui/property_sheets/preferences/advanced_page.hpp>
#include <picotorrent/client/ui/property_sheets/preferences/connection_page.hpp>
#include <picotorrent/client/ui/property_sheets/preferences/downloads_page.hpp>
#include <picotorrent/client/ui/property_sheets/preferences/general_page.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/task_dialog.hpp>
#include <vector>

#include <iphlpapi.h>
#include <prsht.h>
#include <strsafe.h>

namespace ui = picotorrent::client::ui;
namespace prefs = picotorrent::client::ui::property_sheets::preferences;
using picotorrent::core::configuration;
using picotorrent::core::session;
using picotorrent::client::controllers::view_preferences_controller;
using picotorrent::client::ui::dialogs::preferences_dialog;
using picotorrent::client::ui::property_sheets::property_sheet_page;
using picotorrent::client::ui::task_dialog;

view_preferences_controller::view_preferences_controller(const std::shared_ptr<session> &sess,
    const std::shared_ptr<ui::main_window> &wnd)
    : sess_(sess),
    wnd_(wnd),
    adv_page_(std::make_unique<prefs::advanced_page>()),
    conn_page_(std::make_unique<prefs::connection_page>()),
    dl_page_(std::make_unique<prefs::downloads_page>()),
    gen_page_(std::make_unique<prefs::general_page>())
{
    adv_page_->on_apply().connect(std::bind(&view_preferences_controller::on_advanced_apply, this));
    adv_page_->on_init().connect(std::bind(&view_preferences_controller::on_advanced_init, this));

    conn_page_->on_apply().connect(std::bind(&view_preferences_controller::on_connection_apply, this));
    conn_page_->on_init().connect(std::bind(&view_preferences_controller::on_connection_init, this));
    conn_page_->on_validate().connect(std::bind(&view_preferences_controller::on_connection_validate, this));
    conn_page_->set_proxy_type_changed_callback(std::bind(&view_preferences_controller::on_connection_proxy_type_changed, this, std::placeholders::_1));

    dl_page_->on_apply().connect(std::bind(&view_preferences_controller::on_downloads_apply, this));
    dl_page_->on_init().connect(std::bind(&view_preferences_controller::on_downloads_init, this));
    dl_page_->on_validate().connect(std::bind(&view_preferences_controller::on_downloads_validate, this));

    gen_page_->on_apply().connect(std::bind(&view_preferences_controller::on_general_apply, this));
    gen_page_->on_init().connect(std::bind(&view_preferences_controller::on_general_init, this));
}

view_preferences_controller::~view_preferences_controller()
{
}

void view_preferences_controller::execute()
{
    PROPSHEETPAGE p[] =
    {
        *gen_page_,
        *dl_page_,
        *conn_page_,
        *adv_page_
    };

    PROPSHEETHEADER header = { 0 };
    header.dwSize = sizeof(PROPSHEETHEADER);
    header.dwFlags = PSH_NOCONTEXTHELP | PSH_PROPSHEETPAGE;
    header.hwndParent = wnd_->handle();
    header.hInstance = GetModuleHandle(NULL);

    std::wstring caption = TR("preferences");
    header.pszCaption = caption.c_str();

    header.nPages = ARRAYSIZE(p);
    header.nStartPage = 0;
    header.ppsp = (LPCPROPSHEETPAGE)p;
    header.pfnCallback = NULL;

    if (PropertySheet(&header) >= 1)
    {
        sess_->reload_settings();
    }
}

void view_preferences_controller::on_advanced_apply()
{
    configuration &cfg = configuration::instance();
    cfg.set_use_picotorrent_peer_id(adv_page_->use_picotorrent_id());
}

void view_preferences_controller::on_advanced_init()
{
    configuration &cfg = configuration::instance();
    adv_page_->set_use_picotorrent_id(cfg.use_picotorrent_peer_id());
}

void view_preferences_controller::on_downloads_apply()
{
    configuration &cfg = configuration::instance();
    cfg.set_default_save_path(dl_page_->downloads_path());
    cfg.set_prompt_for_save_path(dl_page_->prompt_for_save_path());
}

bool view_preferences_controller::on_downloads_validate()
{
    if (dl_page_->downloads_path().empty())
    {
        dl_page_->show_error_message(
            TR("the_transfers_path_cannot_be_empty"));
        return false;
    }

    return true;
}

void view_preferences_controller::on_connection_apply()
{
    configuration &cfg = configuration::instance();
    cfg.set_listen_address(conn_page_->get_listen_address());
    cfg.set_listen_port(conn_page_->get_listen_port());

    cfg.set_proxy_type((configuration::proxy_type_t)conn_page_->get_proxy_type());
    cfg.set_proxy_host(conn_page_->get_proxy_host());
    cfg.set_proxy_port(conn_page_->get_proxy_port());
    cfg.set_proxy_username(conn_page_->get_proxy_username());
    cfg.set_proxy_password(conn_page_->get_proxy_password());
    cfg.set_proxy_force(conn_page_->get_proxy_force_checked());
    cfg.set_proxy_hostnames(conn_page_->get_proxy_hostnames_checked());
    cfg.set_proxy_peers(conn_page_->get_proxy_peers_checked());
    cfg.set_proxy_trackers(conn_page_->get_proxy_trackers_checked());
}

void view_preferences_controller::on_connection_init()
{
    configuration &cfg = configuration::instance();
    conn_page_->set_listen_address(cfg.listen_address());
    conn_page_->set_listen_port(cfg.listen_port());

    // Add proxy types
    conn_page_->add_proxy_type(TR("none"), configuration::proxy_type_t::none);
    conn_page_->add_proxy_type(TR("http"), configuration::proxy_type_t::http);
    conn_page_->add_proxy_type(TR("http_with_credentials"), configuration::proxy_type_t::http_pw);
    conn_page_->add_proxy_type(TR("i2p"), configuration::proxy_type_t::i2p);
    conn_page_->add_proxy_type(TR("socks4"), configuration::proxy_type_t::socks4);
    conn_page_->add_proxy_type(TR("socks5"), configuration::proxy_type_t::socks5);
    conn_page_->add_proxy_type(TR("socks5_with_credentials"), configuration::proxy_type_t::socks5_pw);

    conn_page_->set_proxy_type(cfg.proxy_type());
    conn_page_->set_proxy_host(cfg.proxy_host());
    if (cfg.proxy_port() > 0) { conn_page_->set_proxy_port(std::to_wstring(cfg.proxy_port())); }
    conn_page_->set_proxy_username(cfg.proxy_username());
    conn_page_->set_proxy_password(cfg.proxy_password());
    conn_page_->set_proxy_force_checked(cfg.proxy_force());
    conn_page_->set_proxy_hostnames_checked(cfg.proxy_hostnames());
    conn_page_->set_proxy_peers_checked(cfg.proxy_peers());
    conn_page_->set_proxy_trackers_checked(cfg.proxy_trackers());

    on_connection_proxy_type_changed(cfg.proxy_type());
}

bool view_preferences_controller::on_connection_validate()
{
    int listenPort = conn_page_->get_listen_port();
    if (listenPort < 1024 || listenPort > 65535)
    {
        conn_page_->show_error_message(TR("invalid_listen_port"));
        return false;
    }

    return true;
}

void view_preferences_controller::on_downloads_init()
{
    configuration &cfg = configuration::instance();
    dl_page_->set_downloads_path(cfg.default_save_path());
    dl_page_->set_prompt_for_save_path(cfg.prompt_for_save_path());
}

void view_preferences_controller::on_connection_proxy_type_changed(int type)
{
    conn_page_->set_proxy_host_enabled(false);
    conn_page_->set_proxy_port_enabled(false);
    conn_page_->set_proxy_username_enabled(false);
    conn_page_->set_proxy_password_enabled(false);
    conn_page_->set_proxy_force_enabled(false);
    conn_page_->set_proxy_hostnames_enabled(false);
    conn_page_->set_proxy_peers_enabled(false);
    conn_page_->set_proxy_trackers_enabled(false);

    switch (type)
    {
    case configuration::proxy_type_t::socks4:
    case configuration::proxy_type_t::socks5:
    case configuration::proxy_type_t::socks5_pw:
    case configuration::proxy_type_t::http:
    case configuration::proxy_type_t::http_pw:
    case configuration::proxy_type_t::i2p:
    {
        conn_page_->set_proxy_host_enabled(true);
        conn_page_->set_proxy_port_enabled(true);
        conn_page_->set_proxy_force_enabled(true);
        conn_page_->set_proxy_peers_enabled(true);
        conn_page_->set_proxy_trackers_enabled(true);
        conn_page_->set_proxy_hostnames_enabled(true);
        break;
    }
    }

    switch(type)
    {
    case configuration::proxy_type_t::socks5_pw:
    case configuration::proxy_type_t::http_pw:
    {
        conn_page_->set_proxy_username_enabled(true);
        conn_page_->set_proxy_password_enabled(true);
        break;
    }
    }
}

void view_preferences_controller::on_general_apply()
{
    int currentLang = i18n::translator::instance().get_current_lang_id();
    int selectedLang = gen_page_->get_selected_language();

    if (currentLang == selectedLang)
    {
        return;
    }

    // Change language!
    i18n::translator::instance().set_current_language(selectedLang);

    // Notify the user about restarting PicoTorrent
    if (should_restart())
    {
        restart();
    }
}

void view_preferences_controller::on_general_init()
{
    std::vector<i18n::translation> langs = i18n::translator::instance().get_available_translations();

    gen_page_->add_languages(langs);
    gen_page_->select_language(i18n::translator::instance().get_current_lang_id());
}

void view_preferences_controller::restart()
{
    TCHAR mod[MAX_PATH];
    TCHAR exe[MAX_PATH];

    GetModuleFileName(NULL, mod, ARRAYSIZE(mod));
    StringCchPrintf(exe, ARRAYSIZE(exe), L"\"%s\" --restart %d", mod, (int)GetCurrentProcessId());

    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    CreateProcess(
        NULL,
        exe,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi);

    wnd_->exit();
}

bool view_preferences_controller::should_restart()
{
    task_dialog dlg;
    dlg.set_common_buttons(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON);
    dlg.set_content(TR("prompt_restart"));
    dlg.set_main_icon(TD_INFORMATION_ICON);
    dlg.set_main_instruction(TR("prompt_restart_title"));
    dlg.set_parent(gen_page_->handle());
    dlg.set_title(L"PicoTorrent");

    return dlg.show() == IDOK;
}
