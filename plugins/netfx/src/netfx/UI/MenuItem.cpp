#include "MenuItem.h"

using PicoTorrent::UI::MenuItem;

MenuItem::MenuItem(HMENU hMenu)
    : _menu(hMenu)
{
}

void MenuItem::Insert(int id, System::String^ text)
{
}
