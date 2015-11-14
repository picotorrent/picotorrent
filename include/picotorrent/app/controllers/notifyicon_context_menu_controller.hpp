#pragma once

#include <memory>
#include <windows.h>

namespace picotorrent
{
namespace core
{
    class session;
}
namespace ui
{
    class main_window;
}
namespace app
{
namespace controllers
{
    class notifyicon_context_menu_controller
    {
    public:
        notifyicon_context_menu_controller(
            const std::shared_ptr<core::session> &sess,
            const std::shared_ptr<ui::main_window> &wnd);

        void execute(const POINT &p);

    private:
        std::shared_ptr<core::session> sess_;
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}