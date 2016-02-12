#pragma once

#include <memory>

namespace picotorrent
{
namespace client
{
namespace ui
{
    class main_window;
}
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
