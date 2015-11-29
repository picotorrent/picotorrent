#pragma once

#include <picotorrent/ui/dialogs/dialog_base.hpp>

namespace picotorrent
{
namespace ui
{
namespace dialogs
{
    class about_dialog : public dialog_base
    {
    public:
        about_dialog();
        ~about_dialog();

    protected:
        BOOL on_command(int, WPARAM, LPARAM);
        BOOL on_init_dialog();
        BOOL on_notify(LPARAM);

    private:
        HFONT title_font_;
    };
}
}
}
