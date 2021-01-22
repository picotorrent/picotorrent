#include "http_session.hpp"

#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>

#include "../commands/command.hpp"
#include "mime_type.hpp"
#include "path_cat.hpp"
#include "websocket_session.hpp"

using json = nlohmann::json;
using pt::http::http_session;

http_session::http_session(
    boost::asio::ip::tcp::socket&& socket,
    sqlite3* db,
    std::shared_ptr<pt::session_manager> const& session_manager,
    std::shared_ptr<std::map<std::string, std::shared_ptr<pt::commands::command>>> const& commands,
    std::shared_ptr<std::string const> const& doc_root)
    : stream_(std::move(socket))
    , m_db(db)
    , m_session_manager(session_manager)
    , m_commands(commands)
    , doc_root_(doc_root)
    , queue_(*this)
{
}

void http_session::run()
{
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    boost::asio::dispatch(
        stream_.get_executor(),
        boost::beast::bind_front_handler(
            &http_session::do_read,
            this->shared_from_this()));
}

void http_session::do_read()
{
    // Construct a new parser for each message
    parser_.emplace();

    // Apply a reasonable limit to the allowed size
    // of the body in bytes to prevent abuse.
    parser_->body_limit(10000000);

    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    // Read a request using the parser-oriented interface
    boost::beast::http::async_read(
        stream_,
        buffer_,
        *parser_,
        boost::beast::bind_front_handler(
            &http_session::on_read,
            shared_from_this()));
}

void http_session::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    namespace http = boost::beast::http;

    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if(ec == http::error::end_of_stream)
    {
        return do_close();
    }

    if(ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Error when reading HTTP request: " << ec.message();
        return;
    }

    BOOST_LOG_TRIVIAL(debug) << "Incoming HTTP request to " << parser_->get().target();

    if (parser_->get().target() == "/api/ws" && boost::beast::websocket::is_upgrade(parser_->get()))
    {
        std::make_shared<websocket_session>(
            stream_.release_socket(),
            m_db,
            m_session_manager)->run(parser_->release());
        return;
    }

    auto req = parser_->release();

    auto const bad_request = [&req](boost::beast::string_view why)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    auto const not_found = [&req](boost::beast::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    auto const server_error = [&req](boost::beast::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    auto const command_response = [&req](std::string_view content)
    {
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "application/json");
        res.keep_alive(req.keep_alive());
        res.body() = content;
        res.prepare_payload();
        return res;
    };

    if (req.target() == "/api/jsonrpc"
        && req.method() == http::verb::post)
    {
        json j;

        try
        {
            j = json::parse(req.body());
        }
        catch (std::exception const& e)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to parse JSON: " << e.what();
        }

        std::string method = j["method"];

        if (m_commands->find(method) == m_commands->end())
        {
            BOOST_LOG_TRIVIAL(error) << "Unknown command: " << method;
            queue_(not_found(method));
        }
        else
        {
            BOOST_LOG_TRIVIAL(debug) << "Running JSONRPC command '" << method << "'";

            json response = m_commands->at(method)->execute(j);
            queue_(command_response(response.dump()));
        }
    }
    else if(doc_root_ == nullptr)
    {
        queue_(not_found(req.target()));
    }
    else
    {
        if (req.target().empty() ||
            req.target()[0] != '/' ||
            req.target().find("..") != boost::beast::string_view::npos)
        {
            queue_(bad_request("Illegal request-target"));
        }
        else
        {
            std::string path = path_cat(*doc_root_, req.target());
            if(req.target().back() == '/') { path.append("index.html"); }

            boost::beast::error_code ec;
            http::file_body::value_type body;
            body.open(path.c_str(), boost::beast::file_mode::scan, ec);

            if(ec == boost::beast::errc::no_such_file_or_directory)
            {
                queue_(not_found(req.target()));
            }
            else if (ec)
            {
                queue_(server_error(ec.message()));
            }
            else
            {
                auto const size = body.size();

                http::response<http::file_body> res{
                    std::piecewise_construct,
                    std::make_tuple(std::move(body)),
                    std::make_tuple(http::status::ok, req.version())};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, mime_type(path));
                res.content_length(size);
                res.keep_alive(req.keep_alive());

                queue_(std::move(res));
            }
        }
    }

    // If we aren't at the queue limit, try to pipeline another request
    if(!queue_.is_full())
    {
        do_read();
    }
}

void http_session::on_write(bool close, boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        // TODO (log)
        return;
    }

    if (close)
    {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return do_close();
    }

    // Inform the queue that a write completed
    if (queue_.on_write())
    {
        // Read another request
        do_read();
    }
}

void http_session::do_close()
{
    // Send a TCP shutdown
    boost::beast::error_code ec;
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}
