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
        explicit peer(const libtorrent::peer_info &pi);
        ~peer();

        std::string client() const;
        int download_rate() const;
        std::string flags_str() const;
        std::string id() const;
        std::string ip() const;
        int port() const;
        int upload_rate() const;

    private:
        std::unique_ptr<libtorrent::peer_info> pi_;
    };
}
}
