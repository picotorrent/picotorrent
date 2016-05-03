#pragma once

#include <picotorrent/client/ui/dialogs/dialog_base.hpp>

namespace picotorrent
{
namespace client
{
namespace ui
{
namespace dialogs
{
    class remote_qr_dialog : public dialog_base
    {
    public:
        remote_qr_dialog();
        ~remote_qr_dialog();

    protected:
        BOOL on_command(int, WPARAM, LPARAM);
        BOOL on_init_dialog();
        BOOL on_notify(LPARAM);
    };
}
}
}
}
