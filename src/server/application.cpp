#include <picotorrent/server/application.hpp>

#ifdef WIN32
#include <windows.h>
#endif

#include <boost/asio.hpp>
#include <picotorrent/common/command_line.hpp>
#include <picotorrent/core/session.hpp>

using picotorrent::common::command_line;
using picotorrent::server::application;

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
    boost::asio::signal_set signals(*io_.get(), SIGINT, SIGTERM);
    signals.async_wait([this](const boost::system::error_code &ec, int signal)
    {
        if (ec)
        {
            // TODO: print error
        }

        io_->stop();
    });

    return (int)io_->run();
}
