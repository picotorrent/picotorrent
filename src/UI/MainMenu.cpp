#include "MainMenu.hpp"

#include "../resources.h"
#include "../Translator.hpp"

using UI::MainMenu;

HMENU MainMenu::Create()
{
    HMENU file = CreateMenu();
    AppendMenu(file, MF_STRING, ID_FILE_ADD_TORRENT, TRW("amp_add_torrent"));
    AppendMenu(file, MF_STRING, ID_FILE_ADD_MAGNET_LINK, TRW("amp_add_magnet_link_s"));
    AppendMenu(file, MF_SEPARATOR, 0, NULL);
    AppendMenu(file, MF_STRING, ID_FILE_EXIT, TRW("amp_exit"));

    HMENU view = CreateMenu();
    AppendMenu(view, MF_STRING, ID_VIEW_PREFERENCES, TRW("amp_preferences"));

    HMENU ext = CreateMenu();

    HMENU help = CreateMenu();
    AppendMenu(help, MF_STRING, ID_HELP_ABOUT, TRW("amp_about"));

    HMENU menuBar = CreateMenu();
    AppendMenu(menuBar, MF_POPUP, (UINT_PTR)file, TRW("amp_file"));
    AppendMenu(menuBar, MF_POPUP, (UINT_PTR)view, TRW("amp_view"));
    AppendMenu(menuBar, MF_POPUP, (UINT_PTR)ext, TRW("amp_extensions"));
    AppendMenu(menuBar, MF_POPUP, (UINT_PTR)help, TRW("amp_help"));

    return menuBar;
}
