#include <picotorrent/net/http_client.hpp>

#include <picotorrent/net/uri.hpp>
#include <sstream>
#include <vector>
#include <windows.h>
#include <winhttp.h>

using picotorrent::net::http_client;
using picotorrent::net::uri;

http_client::http_client()
{
    session_ = WinHttpOpen(
        L"PicoTorrent/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC);
}

http_client::~http_client()
{
    WinHttpCloseHandle(session_);
}

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

struct request_state
{
    HINTERNET request_handle;
    HINTERNET connect_handle;
    std::stringstream response_body;
    std::function<void(std::wstring)> callback;
};

void http_client::get_async(const uri &uri, const std::function<void(std::wstring)> &callback)
{
    request_state *state = new request_state();
    state->callback = callback;

    state->connect_handle = WinHttpConnect(
        session_,
        uri.host_name().c_str(),
        uri.port(),
        NULL);

    state->request_handle = WinHttpOpenRequest(
        state->connect_handle,
        L"POST",
        uri.url_path().c_str(),
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    WinHttpSetStatusCallback(
        state->request_handle,
        &http_client::callback_proxy,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS,
        NULL);

    WinHttpSendRequest(
        state->request_handle,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        NULL,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        (DWORD_PTR)state);
}

void http_client::callback_proxy(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpStatusInformation, DWORD dwStatusInformationLength)
{
    switch (dwInternetStatus)
    {
    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
    {
        break;
    }

    case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
    {
        break;
    }

    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
    {
        request_state *state = (request_state*)dwContext;
        WinHttpReceiveResponse(state->request_handle, NULL);
        break;
    }

    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
    {
        WINHTTP_ASYNC_RESULT *err = static_cast<WINHTTP_ASYNC_RESULT*>(lpStatusInformation);
        break;
    }
    }
}
