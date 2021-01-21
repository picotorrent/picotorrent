#include "http_listener.hpp"

#include <boost/beast.hpp>
#include <boost/log/trivial.hpp>

using pt::http::http_listener;

http_listener::http_listener(
    boost::asio::io_context& ioc,
    boost::asio::ip::tcp::endpoint endpoint,
    sqlite3* db,
    std::shared_ptr<session_manager> const& session_manager,
    std::shared_ptr<std::string const> const& doc_root)
    : m_io(ioc)
    , m_acceptor(boost::asio::make_strand(ioc))
    , m_db(db)
    , m_session_manager(session_manager)
    , m_doc_root(doc_root)
{
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen(boost::asio::socket_base::max_listen_connections);
}

void http_listener::run()
{
    boost::asio::dispatch(
        m_acceptor.get_executor(),
        boost::beast::bind_front_handler(
            &http_listener::do_accept,
            shared_from_this()));
}

void http_listener::do_accept()
{
    m_acceptor.async_accept(
        boost::asio::make_strand(m_io),
        boost::beast::bind_front_handler(
            &http_listener::on_accept,
            shared_from_this()));
}

void http_listener::on_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
{
    if(ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Error when accepting HTTP client: " << ec.message();
    }
    else
    {
        BOOST_LOG_TRIVIAL(debug) << "Incoming HTTP client from " << socket.remote_endpoint();

        /*std::make_shared<http_session>(
            std::move(socket),
            m_db,
            m_session_manager,
            m_doc_root)->run();*/
    }

    do_accept();
}
