#include <picotorrent/net/http_client.hpp>

#include <vector>
#include <windows.h>
#include <winhttp.h>

using picotorrent::net::http_client;

std::string http_client::post(const std::wstring &url, const std::string &content)
{
    HINTERNET hSession = WinHttpOpen(
        L"PicoTorrent/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);

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

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        std::wstring(components.lpszHostName).substr(0, components.dwHostNameLength).c_str(),
        components.nPort,
        NULL);

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"POST",
        components.lpszUrlPath,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    std::wstring h = L"Content-Type: application/json";
    WinHttpAddRequestHeaders(hRequest, h.c_str(), h.size(), WINHTTP_ADDREQ_FLAG_ADD_IF_NEW);

    WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        NULL,
        NULL,
        0,
        content.size(),
        NULL);

    DWORD written = 0;
    WinHttpWriteData(
        hRequest,
        content.c_str(),
        content.size(),
        &written);

    WinHttpReceiveResponse(
        hRequest,
        NULL);

    std::vector<char> buffer;
    DWORD read = 0;

    do
    {
        char temp[1024];
        read = 0;

        WinHttpReadData(
            hRequest,
            temp,
            ARRAYSIZE(temp),
            &read);
        buffer.insert(buffer.end(), temp, temp + read);
    } while (read > 0);

    std::string s(buffer.begin(), buffer.end());

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return s;
}
