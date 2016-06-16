#include "MainMenu.h"

#include "Menu.h"

using PicoTorrent::UI::IMenu;
using PicoTorrent::UI::MainMenu;

MainMenu::MainMenu(HMENU hMenu)
    : _menu(hMenu)
{
}

IMenu^ MainMenu::Help::get()
{
    HMENU hlp = GetSubMenu(_menu, 2);
    return gcnew PicoTorrent::UI::Menu(hlp);
}
