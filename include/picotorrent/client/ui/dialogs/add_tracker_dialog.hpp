#pragma once

#include <picotorrent/client/ui/dialogs/dialog_base.hpp>
#include <picotorrent/core/signals/signal.hpp>
#include <string>
#include <vector>

namespace picotorrent
{
namespace client
{
namespace ui
{
namespace dialogs
{
    class add_tracker_dialog : public dialog_base
    {
    public:
        add_tracker_dialog();

        void close();
        std::wstring get_url();

    protected:
        BOOL on_command(int controlId, WPARAM wParam, LPARAM lParam);
        BOOL on_init_dialog();

    private:
        std::wstring url_;
    };
}
}
}
}
