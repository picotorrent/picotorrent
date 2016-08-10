#include "Menu.h"

#include <msclr/marshal_cppstd.h>

using System::Collections::Generic::IReadOnlyCollection;

using PicoTorrent::UI::Menu;

Menu::Menu(HMENU hMenu)
    : _menu(hMenu)
{
    _marshal = gcnew msclr::interop::marshal_context();
}

void Menu::Insert(int id, System::String^ text)
{
    MENUITEMINFO mi = { sizeof(MENUITEMINFO) };
    mi.fMask = MIIM_ID | MIIM_STRING;
    mi.fType = MFT_STRING;
    mi.wID = id;
    mi.dwTypeData = (LPTSTR)_marshal->marshal_as<LPCTSTR>(text);

    InsertMenuItem(_menu, 0, TRUE, &mi);
}

void Menu::InsertSeparator()
{
    MENUITEMINFO mi = { sizeof(MENUITEMINFO) };
    mi.fMask = MIIM_FTYPE;
    mi.fType = MFT_SEPARATOR;

    InsertMenuItem(_menu, 0, TRUE, &mi);
}

IReadOnlyCollection<PicoTorrent::UI::IMenuItem^>^ Menu::Items::get()
{
    return nullptr;
}
