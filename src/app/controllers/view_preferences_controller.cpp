#include <picotorrent/app/controllers/view_preferences_controller.hpp>

#include <picotorrent/config/configuration.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/ui/dialogs/preferences_dialog.hpp>
#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>
#include <picotorrent/ui/property_sheets/preferences/connection_page.hpp>
#include <picotorrent/ui/property_sheets/preferences/downloads_page.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/resources.hpp>
#include <vector>

#include <iphlpapi.h>
#include <prsht.h>

namespace ui = picotorrent::ui;
namespace prefs = picotorrent::ui::property_sheets::preferences;
using picotorrent::config::configuration;
using picotorrent::core::session;
using picotorrent::app::controllers::view_preferences_controller;
using picotorrent::ui::dialogs::preferences_dialog;
using picotorrent::ui::property_sheets::property_sheet_page;

view_preferences_controller::view_preferences_controller(const std::shared_ptr<session> &sess,
    const std::shared_ptr<ui::main_window> &wnd)
    : sess_(sess),
    wnd_(wnd),
    conn_page_(std::make_unique<prefs::connection_page>()),
    dl_page_(std::make_unique<prefs::downloads_page>())
{
    conn_page_->set_apply_callback(std::bind(&view_preferences_controller::on_connection_apply, this));
    conn_page_->set_init_callback(std::bind(&view_preferences_controller::on_connection_init, this));
    conn_page_->set_proxy_type_changed_callback(std::bind(&view_preferences_controller::on_connection_proxy_type_changed, this, std::placeholders::_1));

    dl_page_->set_apply_callback(std::bind(&view_preferences_controller::on_downloads_apply, this));
    dl_page_->set_init_callback(std::bind(&view_preferences_controller::on_downloads_init, this));
}

view_preferences_controller::~view_preferences_controller()
{
}

void view_preferences_controller::execute()
{
    PROPSHEETPAGE p[] =
    {
        *dl_page_,
        *conn_page_
    };

    PROPSHEETHEADER header = { 0 };
    header.dwSize = sizeof(PROPSHEETHEADER);
    header.dwFlags = PSH_NOCONTEXTHELP | PSH_PROPSHEETPAGE;
    header.hwndParent = wnd_->handle();
    header.hInstance = GetModuleHandle(NULL);
    header.pszCaption = L"Preferences";
    header.nPages = ARRAYSIZE(p);
    header.nStartPage = 0;
    header.ppsp = (LPCPROPSHEETPAGE)p;
    header.pfnCallback = NULL;

    if (PropertySheet(&header) >= 1)
    {
        sess_->reload_settings();
    }
}

void view_preferences_controller::on_downloads_apply()
{
    configuration &cfg = configuration::instance();
    cfg.set_default_save_path(dl_page_->downloads_path());
    cfg.set_prompt_for_save_path(dl_page_->prompt_for_save_path());
}

void view_preferences_controller::on_connection_apply()
{
    configuration &cfg = configuration::instance();
    cfg.set_listen_address(conn_page_->get_listen_address());
    cfg.set_listen_port(conn_page_->get_listen_port());

    cfg.set_proxy_type((configuration::proxy_type_t)conn_page_->get_proxy_type());
    cfg.set_proxy_host(conn_page_->get_proxy_host());
    cfg.set_proxy_port(std::stoi(conn_page_->get_proxy_port()));
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
    conn_page_->add_proxy_type(L"None", configuration::proxy_type_t::none);
    conn_page_->add_proxy_type(L"HTTP", configuration::proxy_type_t::http);
    conn_page_->add_proxy_type(L"HTTP (with credentials)", configuration::proxy_type_t::http_pw);
    conn_page_->add_proxy_type(L"I2P", configuration::proxy_type_t::i2p);
    conn_page_->add_proxy_type(L"SOCKS4", configuration::proxy_type_t::socks4);
    conn_page_->add_proxy_type(L"SOCKS5", configuration::proxy_type_t::socks5);
    conn_page_->add_proxy_type(L"SOCKS5 (with credentials)", configuration::proxy_type_t::socks5_pw);

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

void view_preferences_controller::on_downloads_init()
{
    configuration &cfg = configuration::instance();
    dl_page_->set_downloads_path(cfg.default_save_path());
    dl_page_->set_prompt_for_save_path(cfg.prompt_for_save_path());
}

void view_preferences_controller::on_connection_proxy_type_changed(int type)
{
    /*conn_page_->set_proxy_host(L"");
    conn_page_->set_proxy_port(L"");
    conn_page_->set_proxy_username(L"");
    conn_page_->set_proxy_password(L"");
    conn_page_->set_proxy_force_checked(false);
    conn_page_->set_proxy_hostnames_checked(false);
    conn_page_->set_proxy_peers_checked(false);
    conn_page_->set_proxy_trackers_checked(false);*/

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
