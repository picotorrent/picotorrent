#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <libtorrent/session.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

#include "commands/torrents_add.hpp"
#include "http/http_listener.hpp"
#include "session_manager.hpp"

using pt::commands::torrents_add_command;

int main(int argc, char* argv[])
{
    BOOST_LOG_TRIVIAL(info) << "PicoTorrent Server launching...";

    sqlite3* db;
    sqlite3_open(":memory:", &db);

    boost::asio::io_context io;

    boost::asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(
        [&io](boost::system::error_code const& ec, int signal)
        {
            io.stop();
        });

    auto session = pt::session_manager::load(io, db);

    auto http = std::make_shared<pt::http::http_listener>(
        io,
        boost::asio::ip::tcp::endpoint{ boost::asio::ip::make_address("0.0.0.0"), 6545 },
        db,
        session,
        nullptr);

    http->commands().insert({ "torrents.add", std::make_shared<torrents_add_command>(nullptr) });
    http->run();

    io.run();

    BOOST_LOG_TRIVIAL(info) << "PicoTorrent Server shutting down...";

    sqlite3_close(db);

    return 0;
}
