#pragma once

#include <memory>
#include <picotorrent/common.hpp>
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
        DLL_EXPORT peer(const libtorrent::peer_info &pi);
        DLL_EXPORT peer(const peer &that);

        DLL_EXPORT ~peer();

        DLL_EXPORT std::string client() const;
        DLL_EXPORT int download_rate() const;
        DLL_EXPORT std::string flags_str() const;
        DLL_EXPORT std::string id() const;
        DLL_EXPORT std::string ip() const;
        DLL_EXPORT int port() const;
        DLL_EXPORT int upload_rate() const;

    private:
        std::shared_ptr<libtorrent::peer_info> pi_;
    };
}
}
