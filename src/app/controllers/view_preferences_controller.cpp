#include <picotorrent/app/controllers/view_preferences_controller.hpp>

#include <picotorrent/config/configuration.hpp>
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
using picotorrent::app::controllers::view_preferences_controller;
using picotorrent::ui::dialogs::preferences_dialog;
using picotorrent::ui::property_sheets::property_sheet_page;

view_preferences_controller::view_preferences_controller(const std::shared_ptr<ui::main_window> &wnd)
    : wnd_(wnd),
    conn_page_(std::make_unique<prefs::connection_page>()),
    dl_page_(std::make_unique<prefs::downloads_page>())
{
    conn_page_->set_init_callback(std::bind(&view_preferences_controller::init_connection_page, this));
    conn_page_->set_proxy_type_changed_callback(std::bind(&view_preferences_controller::on_connection_proxy_type_changed, this, std::placeholders::_1));

    dl_page_->set_apply_callback(std::bind(&view_preferences_controller::apply_downloads_page, this));
    dl_page_->set_init_callback(std::bind(&view_preferences_controller::init_downloads_page, this));
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

    switch (PropertySheet(&header))
    {
    case -1:
        break;
    case 0:
        break;
    case 1:
        break;
    }
}

void view_preferences_controller::apply_downloads_page()
{
    configuration &cfg = configuration::instance();
    cfg.set_default_save_path(dl_page_->downloads_path());
    cfg.set_prompt_for_save_path(dl_page_->prompt_for_save_path());
}

void view_preferences_controller::init_connection_page()
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
    on_connection_proxy_type_changed(cfg.proxy_type());
}

void view_preferences_controller::init_downloads_page()
{
    configuration &cfg = configuration::instance();
    dl_page_->set_downloads_path(cfg.default_save_path());
    dl_page_->set_prompt_for_save_path(cfg.prompt_for_save_path());
}

void view_preferences_controller::on_connection_proxy_type_changed(int type)
{
    switch (type)
    {
    case configuration::proxy_type_t::none:
    {
        conn_page_->set_proxy_host_enabled(false);
        conn_page_->set_proxy_port_enabled(false);
        break;
    }
    }
}
