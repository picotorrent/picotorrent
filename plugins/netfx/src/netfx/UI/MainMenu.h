#pragma once

#include <windows.h>

namespace PicoTorrent
{
namespace UI
{
    ref class MainMenu : public IMainMenu
    {
    public:
        MainMenu(HMENU);

        virtual property IMenu^ Help { IMenu^ get(); }

    private:
        HMENU _menu;
    };
}
}

