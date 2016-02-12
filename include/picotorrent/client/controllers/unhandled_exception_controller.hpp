#pragma once

#include <memory>
#include <string>

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
    class unhandled_exception_controller
    {
    public:
        unhandled_exception_controller(const std::shared_ptr<ui::main_window> &wnd, const std::string &stacktrace);
        void execute();

    private:
        std::shared_ptr<ui::main_window> wnd_;
        std::string stacktrace_;
    };
}
}
}
