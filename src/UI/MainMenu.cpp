#include "MainMenu.hpp"

#include "../resources.h"

using UI::MainMenu;

MainMenu::MainMenu()
{
    CreateMenu();

    CMenu fileMenu;
    if (fileMenu.CreateMenu())
    {
        fileMenu.AppendMenu(MF_STRING, ID_FILE_ADD_TORRENT, TEXT("Add torrent(s)"));
        fileMenu.AppendMenu(MF_STRING, ID_FILE_ADD_MAGNET_LINK, TEXT("Add magnet link(s)"));
        fileMenu.AppendMenu(MF_SEPARATOR);
        fileMenu.AppendMenu(MF_STRING, ID_FILE_EXIT, TEXT("Exit"));
    }

    CMenu viewMenu;
    if (viewMenu.CreateMenu())
    {
        viewMenu.AppendMenu(MF_STRING, ID_VIEW_PREFERENCES, TEXT("Preferences"));
    }

    CMenu helpMenu;
    if (helpMenu.CreateMenu())
    {
        helpMenu.AppendMenu(MF_STRING, ID_HELP_ABOUT, TEXT("About"));
    }

    AppendMenu(MF_POPUP, fileMenu, TEXT("File"));
    AppendMenu(MF_POPUP, viewMenu, TEXT("View"));
    AppendMenu(MF_POPUP, helpMenu, TEXT("Help"));
}
