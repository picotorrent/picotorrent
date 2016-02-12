#include <picotorrent/client/net/http_response.hpp>

using picotorrent::client::net::http_response;

http_response::http_response(int status_code, const std::string &content)
    : content_(content),
    status_(status_code)
{
}

std::string http_response::content() const
{
    return content_;
}

bool http_response::is_success_status() const
{
    return status_ == 200
        || status_ == 202;
}

int http_response::status_code() const
{
    return status_;
}
