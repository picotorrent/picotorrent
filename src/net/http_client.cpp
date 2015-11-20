#include <picotorrent/net/http_client.hpp>

#include <picotorrent/net/http_response.hpp>
#include <picotorrent/net/uri.hpp>
#include <sstream>
#include <vector>
#include <windows.h>
#include <winhttp.h>

using picotorrent::net::http_client;
using picotorrent::net::http_response;
using picotorrent::net::uri;

class request_state
{
public:
    request_state()
        : request_handle(NULL),
        connect_handle(NULL)
    {
    }

    ~request_state()
    {
        if (request_handle != NULL)
        {
            WinHttpCloseHandle(request_handle);
        }

        if (connect_handle != NULL)
        {
            WinHttpCloseHandle(connect_handle);
        }
    }

    HINTERNET request_handle;
    HINTERNET connect_handle;
    std::stringstream response_body;
    DWORD data_size = 0;
    DWORD total_size = 0;
    int response_status_code;
    std::function<void(const http_response&)> callback;
};

http_client::http_client()
{
    session_ = WinHttpOpen(
        L"PicoTorrent/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC);

    WinHttpSetStatusCallback(
        session_,
        &http_client::callback_proxy,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS,
        NULL);
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

void http_client::get_async(const uri &uri, const std::function<void(const http_response&)> &callback)
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
        L"GET",
        uri.url_path().c_str(),
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

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
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    {
        request_state *state = (request_state*)dwContext;
        state->data_size = *((LPDWORD)lpStatusInformation);

        if (state->data_size == 0)
        {
            http_response response(
                state->response_status_code,
                state->response_body.str());

            state->callback(response);
            delete state;
        }
        else
        {
            char *buf = new char[state->data_size + 1];
            WinHttpReadData(
                state->request_handle,
                static_cast<LPVOID>(buf),
                state->data_size,
                NULL);
        }

        break;
    }

    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
    {
        request_state *state = (request_state*)dwContext;

        // Query headers
        std::wstring status = read_header(state->request_handle, WINHTTP_QUERY_STATUS_CODE);
        state->response_status_code = std::stoi(status);

        WinHttpQueryDataAvailable(
            state->request_handle,
            NULL);
        break;
    }

    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    {
        request_state *state = (request_state*)dwContext;

        if (dwStatusInformationLength != 0)
        {
            char* buf = static_cast<char*>(lpStatusInformation);
            state->response_body << std::string(buf, dwStatusInformationLength);
            delete[] buf;

            state->total_size += state->data_size;

            WinHttpQueryDataAvailable(
                state->request_handle,
                NULL);
        }
        break;
    }

    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
    {
        request_state *state = (request_state*)dwContext;
        WinHttpReceiveResponse(state->request_handle, NULL);
        break;
    }
    }
}

std::wstring http_client::read_header(HINTERNET hRequest, DWORD dwHeader)
{
    DWORD bufLen;

    WinHttpQueryHeaders(
        hRequest,
        dwHeader,
        WINHTTP_HEADER_NAME_BY_INDEX,
        WINHTTP_NO_OUTPUT_BUFFER,
        &bufLen,
        WINHTTP_NO_HEADER_INDEX);

    std::wstring res(L"\0", bufLen);

    WinHttpQueryHeaders(
        hRequest,
        dwHeader,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &res[0],
        &bufLen,
        WINHTTP_NO_HEADER_INDEX);

    return res;
}
