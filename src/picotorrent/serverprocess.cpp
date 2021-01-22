#include "serverprocess.hpp"

#include <boost/beast.hpp>
#include <boost/process.hpp>
#include <boost/process/windows.hpp>

#include "clientwebsocket.hpp"

using json = nlohmann::json;
using pt::ServerProcess;

class HttpClient : public std::enable_shared_from_this<HttpClient>
{
public:
    HttpClient(boost::asio::io_context& io)
        : m_stream(boost::asio::make_strand(io))
    {
    }

    void Post(nlohmann::json& j)
    {
        m_req.version(11);
        m_req.method(boost::beast::http::verb::post);
        m_req.target("/api/jsonrpc");
        m_req.set(boost::beast::http::field::host, "localhost");
        m_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        m_req.body() = j.dump();
        m_req.prepare_payload();

        m_stream.async_connect(
            boost::asio::ip::tcp::endpoint{ boost::asio::ip::make_address("127.0.0.1"), 6545 },
            boost::beast::bind_front_handler(
                &HttpClient::OnConnect,
                shared_from_this()));
    }

private:
    void OnConnect(boost::beast::error_code ec)
    {
        if (ec)
        {
            OutputDebugStringA(ec.message().c_str());
            OutputDebugStringA("\n");
            return;
        }

        OutputDebugStringA("HttpClient: Connected\n");

        // Set a timeout on the operation
        m_stream.expires_after(std::chrono::seconds(30));

        OutputDebugStringA(m_req.body().c_str());

        // Send the HTTP request to the remote host
        boost::beast::http::async_write(
            m_stream,
            m_req,
            boost::beast::bind_front_handler(
                &HttpClient::OnWrite,
                shared_from_this()));
    }

    void OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        if (ec)
        {
            OutputDebugStringA(ec.message().c_str());
            OutputDebugStringA("\n");
            return;
        }

        OutputDebugStringA("HttpClient: Wrote ");
        OutputDebugStringA(std::to_string(bytes_transferred).c_str());
        OutputDebugStringA(" byte(s)\n");

        boost::beast::http::async_read(
            m_stream,
            m_buffer,
            m_res,
            boost::beast::bind_front_handler(
                &HttpClient::OnRead,
                shared_from_this()));
    }

    void OnRead(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        if (ec)
        {
            OutputDebugStringA(ec.message().c_str());
            OutputDebugStringA("\n");
            return;
        }

        OutputDebugStringA("HttpClient: Read ");
        OutputDebugStringA(std::to_string(bytes_transferred).c_str());
        OutputDebugStringA(" byte(s)\n");
        OutputDebugStringA(m_res.body().c_str());
        OutputDebugStringA("\n");
    }

    boost::beast::tcp_stream m_stream;
    boost::beast::flat_buffer m_buffer;
    boost::beast::http::request<boost::beast::http::string_body> m_req;
    boost::beast::http::response<boost::beast::http::string_body> m_res;
};

ServerProcess::ServerProcess()
{
    m_thread = std::thread(std::bind(&ServerProcess::Run, this));
}

ServerProcess::~ServerProcess()
{
    m_io.stop();
    m_thread.join();
}

void ServerProcess::RPC(std::string const& method, nlohmann::json& params)
{
    json rpc = {
        { "method", method },
        { "params", params }
    };

    std::make_shared<HttpClient>(m_io)->Post(rpc);
}

void ServerProcess::Run()
{
    std::vector<char> buf(4096);
    boost::process::async_pipe ap(m_io);

    boost::process::child child(
        "",
        //boost::process::windows::hide,
        //boost::process::std_out > ap,
        m_io,
        boost::process::on_exit(
            [](int code, std::error_code)
            {
                OutputDebugStringA(std::to_string(code).c_str());
            }));

    auto read = [=](const boost::system::error_code&, std::size_t size)
    {
        std::string out(buf.begin(), buf.begin() + size);
        OutputDebugStringA(out.c_str());
        OutputDebugStringA("\n");
        //ap.async_read_some(boost::asio::buffer(buf), read);
    };

    ap.async_read_some(boost::asio::buffer(buf), read);

    std::make_shared<ClientWebSocket>(m_io)->Run();

    m_io.run();

    child.terminate(); // TODO: shut down cleanly
}
