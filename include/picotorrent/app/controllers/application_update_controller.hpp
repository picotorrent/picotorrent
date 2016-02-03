#pragma once

#include <memory>
#include <picotorrent/ui/main_window.hpp>
#include <string>

namespace picotorrent
{
namespace net
{
    class http_client;
    class http_response;
    class uri;
}
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
        ~application_update_controller();

        void execute(bool forced = false);

    protected:
        void notify(const std::wstring &title, const net::uri &uri, const std::wstring &version);
        void on_response(const net::http_response &response, bool forced);

    private:
        std::shared_ptr<net::http_client> http_;
        std::shared_ptr<ui::main_window> wnd_;
    };
}
}
}
