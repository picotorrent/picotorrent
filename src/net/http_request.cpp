#include <picotorrent/net/http_request.hpp>

using picotorrent::net::http_request;

http_request::http_request(const std::wstring &method)
    : method_(method)
{
}
