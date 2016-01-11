#include <picotorrent/app/controllers/view_preferences_controller.hpp>

#include <picotorrent/config/configuration.hpp>
#include <picotorrent/ui/dialogs/preferences_dialog.hpp>
#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>
#include <picotorrent/ui/property_sheets/preferences/downloads_page.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/resources.hpp>
#include <vector>

#include <prsht.h>

namespace ui = picotorrent::ui;
namespace prefs = picotorrent::ui::property_sheets::preferences;
using picotorrent::config::configuration;
using picotorrent::app::controllers::view_preferences_controller;
using picotorrent::ui::dialogs::preferences_dialog;
using picotorrent::ui::property_sheets::property_sheet_page;

view_preferences_controller::view_preferences_controller(const std::shared_ptr<ui::main_window> &wnd)
    : wnd_(wnd)
{
}

void view_preferences_controller::execute()
{
    /*pages[0] = { 0 };
    pages[0].dwSize = sizeof(PROPSHEETPAGE);
    pages[0].dwFlags = PSP_USETITLE;
    pages[0].hInstance = GetModuleHandle(NULL);
    pages[0].pszTemplate = MAKEINTRESOURCE(6767);
    pages[0].pfnDlgProc = DialogProc;
    pages[0].pszTitle = MAKEINTRESOURCE(IDS_PREFS_DOWNLOADS_TITLE);
    pages[0].lParam = 0;
    pages[0].pfnCallback = NULL;

    pages[1] = { 0 };
    pages[1].dwSize = sizeof(PROPSHEETPAGE);
    pages[1].dwFlags = PSP_USETITLE;
    pages[1].hInstance = GetModuleHandle(NULL);
    pages[1].pszTemplate = MAKEINTRESOURCE(6768);
    pages[1].pfnDlgProc = DialogProc;
    pages[1].pszTitle = MAKEINTRESOURCE(IDS_PREFS_CONNECTION_TITLE);
    pages[1].lParam = 0;
    pages[1].pfnCallback = NULL;

    pages[2] = { 0 };
    pages[2].dwSize = sizeof(PROPSHEETPAGE);
    pages[2].dwFlags = PSP_USETITLE;
    pages[2].hInstance = GetModuleHandle(NULL);
    pages[2].pszTemplate = MAKEINTRESOURCE(6769);
    pages[2].pfnDlgProc = DialogProc;
    pages[2].pszTitle = MAKEINTRESOURCE(IDS_PREFS_ADVANCED_TITLE);
    pages[2].lParam = 0;
    pages[2].pfnCallback = NULL;
    */

    configuration &cfg = configuration::instance();

    prefs::downloads_page dl;
    dl.set_init_callback([&cfg, &dl]()
    {
        dl.set_downloads_path(cfg.default_save_path());
    });

    PROPSHEETPAGE p[1];
    p[0] = dl;

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

    INT_PTR res = PropertySheet(&header);

    if (res == -1)
    {
        DWORD err = GetLastError();
        printf("");
    }
}
