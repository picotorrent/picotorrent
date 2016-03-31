#include <picotorrent/client/net/uri.hpp>

#include <picotorrent/client/string_operations.hpp>

#include <windows.h>
#include <winhttp.h>

using picotorrent::client::net::uri;

uri::uri(const std::string &url)
    : raw_(url)
{
    URL_COMPONENTS components = { 0 };
    components.dwStructSize = sizeof(URL_COMPONENTS);
    components.dwSchemeLength = (DWORD)-1;
    components.dwHostNameLength = (DWORD)-1;
    components.dwUrlPathLength = (DWORD)-1;
    components.dwExtraInfoLength = (DWORD)-1;

    std::wstring u = to_wstring(url);

    WinHttpCrackUrl(
        u.c_str(),
        (DWORD)u.size(),
        0,
        &components);

    hostName_ = to_string(std::wstring(components.lpszHostName, components.dwHostNameLength));
    port_ = components.nPort;
    urlPath_ = to_string(components.lpszUrlPath);
}

std::string uri::host_name() const
{
    return hostName_;
}

int uri::port() const
{
    return port_;
}

std::string uri::raw() const
{
    return raw_;
}

std::string uri::url_path() const
{
    return urlPath_;
}
