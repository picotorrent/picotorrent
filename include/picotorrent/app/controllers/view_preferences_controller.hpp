#pragma once

#include <memory>
#include <string>

namespace picotorrent
{
namespace ui
{
    class main_window;
    class preferences_dialog;
}
namespace app
{
namespace controllers
{
    class view_preferences_controller
    {
    public:
        view_preferences_controller(const std::shared_ptr<ui::main_window> &wnd);
        void execute();
        void init_dlg(ui::preferences_dialog &dlg);
        void ok_dlg(ui::preferences_dialog &dlg);

    private:
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
