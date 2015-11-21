#pragma once

#include <string>

namespace picotorrent
{
namespace net
{
    class uri
    {
    public:
        uri(const std::wstring &url);

        std::wstring host_name() const;
        int port() const;
        std::wstring raw() const;
        std::wstring url_path() const;

    private:
        std::wstring scheme_;
        std::wstring hostName_;
        std::wstring urlPath_;
        int port_;
        std::wstring raw_;
    };
}
}
