#pragma once

#include <string>

namespace picotorrent
{
namespace client
{
namespace net
{
    class uri
    {
    public:
        uri(const std::string &url);

        std::string host_name() const;
        int port() const;
        std::string raw() const;
        std::string url_path() const;

    private:
        std::string scheme_;
        std::string hostName_;
        std::string urlPath_;
        int port_;
        std::string raw_;
    };
}
}
}
