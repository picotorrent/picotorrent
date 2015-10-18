#pragma once

#include <string>

namespace picotorrent
{
namespace net
{
    class http_client
    {
    public:
        std::string post(const std::wstring &url, const std::string &content);
    };
}
}