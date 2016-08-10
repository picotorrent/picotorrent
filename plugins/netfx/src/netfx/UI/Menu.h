#pragma once

#include <windows.h>
#include <msclr/marshal_cppstd.h>

namespace PicoTorrent
{
namespace UI
{
    ref class Menu : IMenu
    {
    public:
        Menu(HMENU);

        virtual void Insert(int id, System::String^ text);

        virtual void InsertSeparator();

        virtual property System::Collections::Generic::IReadOnlyCollection<IMenuItem^>^ Items { System::Collections::Generic::IReadOnlyCollection<IMenuItem^>^ get(); }

    private:
        HMENU _menu;
        msclr::interop::marshal_context^ _marshal;
    };

}
}
