#include <picotorrent/server/application.hpp>

#ifdef WIN32
#include <windows.h>
#endif

#include <boost/asio.hpp>
#include <picotorrent/common/command_line.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/server/hosting/console_host.hpp>
#include <picotorrent/server/hosting/windows_service_host.hpp>

using picotorrent::common::command_line;
using picotorrent::server::application;
using picotorrent::server::hosting::console_host;
using picotorrent::server::hosting::windows_service_host;

void application::allocate_console()
{
#ifdef WIN32
    if (AllocConsole())
    {
        FILE *conin, *conout;

        freopen_s(&conin, "conin$", "r", stdin);
        freopen_s(&conout, "conout$", "w", stdout);
        freopen_s(&conout, "conout$", "w", stderr);
    }
#endif
}

bool application::on_init()
{
    io_ = std::make_shared<boost::asio::io_service>();
    return true;
}

void application::on_notifications_available()
{
    io_->post([this]()
    {
        this->get_session()
            ->post_updates();
    });
}

int application::on_run(const command_line &cmd)
{
    if (cmd.alloc_console())
    {
        console_host host;
        return host.run(io_);
    }
    else
    {
        windows_service_host host;
        return host.run(io_);
    }
}
