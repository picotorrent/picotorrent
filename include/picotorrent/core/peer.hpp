#pragma once

#include <memory>
#include <string>

namespace libtorrent
{
    struct peer_info;
}

namespace picotorrent
{
namespace core
{
    class peer
    {
    public:
        peer(const libtorrent::peer_info &pi);
        peer(const peer &that);

        ~peer();

        std::string client() const;
        int download_rate() const;
        std::string flags_str() const;
        std::string id() const;
        std::string ip() const;
        int port() const;
        int upload_rate() const;

    private:
        std::shared_ptr<libtorrent::peer_info> pi_;
    };
}
}
