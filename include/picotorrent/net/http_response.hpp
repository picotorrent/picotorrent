#pragma once

#include <string>

namespace picotorrent
{
namespace net
{
    class http_response
    {
    public:
        http_response(int status_code, const std::string &content);

        std::string content() const;
        bool is_success_status() const;
        int status_code() const;

    private:
        std::string content_;
        int status_;
    };
}
}
