#include <picotorrent/net/uri.hpp>

#include <windows.h>
#include <winhttp.h>

using picotorrent::net::uri;

uri::uri(const std::wstring &url)
{
    URL_COMPONENTS components = { 0 };
    components.dwStructSize = sizeof(URL_COMPONENTS);
    components.dwSchemeLength = (DWORD)-1;
    components.dwHostNameLength = (DWORD)-1;
    components.dwUrlPathLength = (DWORD)-1;
    components.dwExtraInfoLength = (DWORD)-1;

    WinHttpCrackUrl(
        url.c_str(),
        url.size(),
        0,
        &components);

    hostName_ = std::wstring(components.lpszHostName, components.dwHostNameLength);
    port_ = components.nPort;
    urlPath_ = components.lpszUrlPath;
}

std::wstring uri::host_name() const
{
    return hostName_;
}

int uri::port() const
{
    return port_;
}

std::wstring uri::url_path() const
{
    return urlPath_;
}
