#pragma once

#include <memory>
#include <picotorrent/ui/main_window.hpp>

namespace picotorrent
{
namespace ui
{
    class main_window;
}
namespace app
{
namespace controllers
{
    class application_update_controller
    {
    public:
        application_update_controller(const std::shared_ptr<ui::main_window> &wnd);

        void execute();

    private:
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
