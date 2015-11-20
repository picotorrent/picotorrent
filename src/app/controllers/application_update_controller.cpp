#include <picotorrent/app/controllers/application_update_controller.hpp>

#include <picotorrent/net/http_client.hpp>
#include <picotorrent/net/uri.hpp>
#include <picotorrent/ui/main_window.hpp>

using picotorrent::app::controllers::application_update_controller;
using picotorrent::net::http_client;
using picotorrent::net::uri;
using picotorrent::ui::main_window;

application_update_controller::application_update_controller(
    const std::shared_ptr<main_window> &wnd)
    : wnd_(wnd)
{
}

void application_update_controller::execute()
{
    http_client client;
    uri api(L"https://api.github.com/repos/picotorrent/picotorrent/releases/latest");

    client.get_async(api, std::bind(&application_update_controller::on_response, this, std::placeholders::_1));
}

void application_update_controller::on_response(std::wstring content)
{
}
