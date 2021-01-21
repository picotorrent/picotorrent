#pragma once

#include <memory>

#include <boost/asio.hpp>
#include <sqlite3.h>

namespace pt
{
    class session_manager;
}

namespace pt::http
{
    class http_listener : public std::enable_shared_from_this<http_listener>
    {
    public:
        http_listener(
            boost::asio::io_context& ioc,
            boost::asio::ip::tcp::endpoint endpoint,
            sqlite3* db,
            std::shared_ptr<session_manager> const& session_manager,
            std::shared_ptr<std::string const> const& doc_root);

        void run();

    private:
        void do_accept();
        void on_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

        boost::asio::io_context& m_io;
        boost::asio::ip::tcp::acceptor m_acceptor;
        sqlite3* m_db;
        std::shared_ptr<session_manager> m_session_manager;
        std::shared_ptr<std::string const> m_doc_root;
    };
}
