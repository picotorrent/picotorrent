#pragma once

#include <map>
#include <memory>
#include <vector>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <sqlite3.h>


namespace pt { class session_manager; }
namespace pt::commands { class command;  }

namespace pt::http
{
    class http_session : public std::enable_shared_from_this<http_session>
    {
        // This queue is used for HTTP pipelining.
        class queue
        {
            enum
            {
                // Maximum number of responses we will queue
                limit = 8
            };

            // The type-erased, saved work item
            struct work
            {
                virtual ~work() = default;
                virtual void operator()() = 0;
            };

            http_session& self_;
            std::vector<std::unique_ptr<work>> items_;

        public:
            explicit queue(http_session& self)
                : self_(self)
            {
                static_assert(limit > 0, "queue limit must be positive");
                items_.reserve(limit);
            }

            // Returns `true` if we have reached the queue limit
            bool is_full() const
            {
                return items_.size() >= limit;
            }

            // Called when a message finishes sending
            // Returns `true` if the caller should initiate a read
            bool on_write()
            {
                BOOST_ASSERT(!items_.empty());
                auto const was_full = is_full();
                items_.erase(items_.begin());
                if(! items_.empty())
                    (*items_.front())();
                return was_full;
            }

            // Called by the HTTP handler to send a response.
            template<bool isRequest, class Body, class Fields>
            void operator()(boost::beast::http::message<isRequest, Body, Fields>&& msg)
            {
                // This holds a work item
                struct work_impl : work
                {
                    http_session& self_;
                    boost::beast::http::message<isRequest, Body, Fields> msg_;

                    work_impl(
                        http_session& self,
                        boost::beast::http::message<isRequest, Body, Fields>&& msg)
                        : self_(self)
                        , msg_(std::move(msg))
                    {
                    }

                    void operator()()
                    {
                        boost::beast::http::async_write(
                            self_.stream_,
                            msg_,
                            boost::beast::bind_front_handler(
                                &http_session::on_write,
                                self_.shared_from_this(),
                                msg_.need_eof()));
                    }
                };

                // Allocate and store the work
                items_.push_back(
                    std::make_unique<work_impl>(self_, std::move(msg)));

                // If there was no previous work, start this one
                if(items_.size() == 1)
                    (*items_.front())();
            }
        };

    public:
        http_session(
            boost::asio::ip::tcp::socket&& socket,
            sqlite3* db,
            std::shared_ptr<session_manager> const& session_manager,
            std::shared_ptr<std::map<std::string, std::shared_ptr<pt::commands::command>>> const& commands,
            std::shared_ptr<std::string const> const& doc_root);

        void run();

    private:
        void do_read();
        void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
        void on_write(bool close, boost::beast::error_code ec, std::size_t bytes_transferred);
        void do_close();

        boost::beast::tcp_stream stream_;
        boost::beast::flat_buffer buffer_;
        sqlite3* m_db;
        std::shared_ptr<session_manager> m_session_manager;
        std::shared_ptr<std::string const> doc_root_;
        std::shared_ptr<std::map<std::string, std::shared_ptr<pt::commands::command>>> m_commands;
        queue queue_;

        // The parser is stored in an optional container so we can
        // construct it from scratch it at the beginning of each new message.
        boost::optional<boost::beast::http::request_parser<boost::beast::http::string_body>> parser_;
    };
}
