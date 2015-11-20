#pragma once

#include <memory>
#include <picotorrent/ui/main_window.hpp>
#include <string>

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

    protected:
        void on_response(std::wstring content);

    private:
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
