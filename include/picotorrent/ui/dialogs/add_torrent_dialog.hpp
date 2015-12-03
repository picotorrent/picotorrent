#pragma once

#include <picotorrent/ui/dialogs/dialog_base.hpp>

#include <functional>
#include <memory>
#include <string>

namespace picotorrent
{
namespace ui
{
namespace controls
{
    class list_view;
}
namespace dialogs
{
    class add_torrent_dialog : public dialog_base
    {
    public:
        add_torrent_dialog();

        void add_torrent(const std::wstring &name);
        void set_init_callback(const std::function<void()> &callback);
        void set_save_path(const std::wstring &path);
        void set_selected_item(int item);
        void set_size(const std::wstring &friendly_size);

    protected:
        BOOL on_command(int, WPARAM, LPARAM);
        BOOL on_init_dialog();

    private:
        std::function<void()> init_cb_;
        std::shared_ptr<controls::list_view> files_;
        HWND combo_;
        HWND save_path_;
        HWND size_;
    };
}
}
}
