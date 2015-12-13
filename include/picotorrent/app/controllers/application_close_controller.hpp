#pragma once

#include <memory>

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
    class application_close_controller
    {
    public:
        application_close_controller(const std::shared_ptr<ui::main_window> &wnd);
        bool execute();

    protected:
        bool prompt_for_exit();

    private:
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
