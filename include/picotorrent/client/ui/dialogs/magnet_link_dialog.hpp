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
    class magnet_link_dialog : public dialog_base
    {
    public:
        magnet_link_dialog();

        void close();
        void disable_actions();
        std::vector<std::wstring> get_links();
        core::signals::signal_connector<void, void>& on_add_links();
        void start_progress();
        void update_status_text(int current, int total);

    protected:
        BOOL on_command(int controlId, WPARAM wParam, LPARAM lParam);
        BOOL on_init_dialog();

    private:
        core::signals::signal<void, void> on_add_links_;
    };
}
}
}
}
