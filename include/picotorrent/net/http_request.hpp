#pragma once

#include <string>

namespace picotorrent
{
namespace net
{
    class http_request
    {
    public:
        http_request(const std::wstring &method);

        std::wstring& content() const;
        void set_content(const std::wstring &c);

    private:
        std::wstring method_;
    };
}
}
