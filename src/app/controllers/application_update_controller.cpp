#include <picotorrent/app/controllers/application_update_controller.hpp>

#include <picotorrent/ui/main_window.hpp>

using picotorrent::app::controllers::application_update_controller;
using picotorrent::ui::main_window;

application_update_controller::application_update_controller(
    const std::shared_ptr<main_window> &wnd)
    : wnd_(wnd)
{
}

void application_update_controller::execute()
{
}
