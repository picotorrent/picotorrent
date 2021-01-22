#pragma once

#include <thread>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

namespace pt
{
    class ServerProcess
    {
    public:
        ServerProcess();
        ~ServerProcess();

        void RPC(std::string const& cmd, nlohmann::json& json);

    private:
        void Run();

        boost::asio::io_context m_io;
        std::thread m_thread;
    };
}
