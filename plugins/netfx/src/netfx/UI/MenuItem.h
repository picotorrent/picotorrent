#pragma once

#include <windows.h>

namespace PicoTorrent
{
namespace UI
{
    ref class MenuItem : public IMenuItem
    {
    public:
        MenuItem(HMENU);

        virtual void Insert(int id, System::String^ text);

    private:
        HMENU _menu;
    };
}
}
