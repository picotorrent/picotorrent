#include <picotorrent/server/hosting/console_host.hpp>

#include <boost/asio.hpp>

using picotorrent::server::hosting::console_host;

int console_host::run(const std::shared_ptr<boost::asio::io_service> &io)
{
    boost::asio::signal_set signals(*io, SIGINT, SIGTERM);
    signals.async_wait([io](const boost::system::error_code &ec, int signal)
    {
        if (ec)
        {
            // TODO: print error
        }

        io->stop();
    });

    boost::system::error_code ec;
    io->run(ec);

    return ec.value();
}
