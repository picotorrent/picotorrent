#include <picotorrent/app/controllers/application_update_controller.hpp>
#include <picotorrent/picojson.hpp>
#include <picotorrent/logging/log.hpp>
#include <picotorrent/net/http_client.hpp>
#include <picotorrent/net/http_response.hpp>
#include <picotorrent/net/uri.hpp>
#include <picotorrent/ui/main_window.hpp>

using namespace picotorrent::logging;

using picotorrent::app::controllers::application_update_controller;
using picotorrent::net::http_client;
using picotorrent::net::http_response;
using picotorrent::net::uri;
using picotorrent::ui::main_window;

application_update_controller::application_update_controller(
    const std::shared_ptr<main_window> &wnd)
    : http_(std::make_shared<http_client>()),
    wnd_(wnd)
{
}

application_update_controller::~application_update_controller()
{
}

void application_update_controller::execute()
{
    uri api(L"https://api.github.com/repos/picotorrent/picotorrent/releases/latest");
    http_->get_async(api, std::bind(&application_update_controller::on_response, this, std::placeholders::_1));
}

void application_update_controller::on_response(const http_response &response)
{
    if (!response.is_success_status())
    {
        LOG(error) << "GitHub returned status " << response.status_code();
        return;
    }

    picojson::value v;
    std::string err = picojson::parse(v, response.content());

    if (!err.empty())
    {
        LOG(error) << "Error when parsing release JSON: " << err;
        return;
    }

    picojson::object obj = v.get<picojson::object>();
    std::string version = obj["tag_name"].get<std::string>();

    printf("%s\n", response.content().c_str());
}
