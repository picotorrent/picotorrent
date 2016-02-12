#pragma once

#include <picotorrent/client/ui/dialogs/dialog_base.hpp>
#include <string>

namespace picotorrent
{
namespace client
{
namespace ui
{
namespace dialogs
{
    class preferences_dialog : public dialog_base
    {
    public:
        preferences_dialog();

        std::wstring default_save_path();
        int listen_port();
        bool prompt_for_save_path();
        void set_default_save_path(const std::wstring &path);
        void set_listen_port(int port);
        void set_prompt_for_save_path(bool prompt);

    protected:
        BOOL on_command(int, WPARAM, LPARAM);
        BOOL on_init_dialog();

    private:
        std::wstring save_path_;
        int listen_port_;
        bool prompt_;
    };
}
}
}
}
