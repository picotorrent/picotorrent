#include "httpclient.hpp"

#include <sstream>

using pt::http::HttpClient;

struct State
{
    HINTERNET hConnect;
    HINTERNET hRequest;
    std::stringstream response;
    DWORD dataSize = 0;
    DWORD totalSize = 0;
    int statusCode = 0;
    std::function<void(pt::http::HttpResponse)> callback;
};

HttpClient::HttpClient()
{
    m_hSession = WinHttpOpen(
        L"PicoTorrent/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC);

    WinHttpSetStatusCallback(
        m_hSession,
        &HttpClient::StatusCallbackProxy,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS,
        NULL);
}

HttpClient::~HttpClient()
{
    WinHttpCloseHandle(m_hSession);
}

void HttpClient::GetAsync(const std::wstring& url, std::function<void(pt::http::HttpResponse)> const& callback)
{
    // Crack URI
    URL_COMPONENTS uc = { sizeof(URL_COMPONENTS) };
    uc.dwSchemeLength = (DWORD)-1;
    uc.dwHostNameLength = (DWORD)-1;
    uc.dwUrlPathLength = (DWORD)-1;
    uc.dwExtraInfoLength = (DWORD)-1;
    WinHttpCrackUrl(url.c_str(), (DWORD)url.size(), 0, &uc);

    std::wstring scheme(uc.lpszScheme, uc.dwSchemeLength);
    bool secure = scheme == L"https";

    State* state = new State();
    state->callback = callback;

    std::wstring host(uc.lpszHostName, uc.dwHostNameLength);
    state->hConnect = WinHttpConnect(m_hSession, host.c_str(), uc.nPort, NULL);
    state->hRequest = WinHttpOpenRequest(state->hConnect, L"GET", uc.lpszUrlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, secure ? WINHTTP_FLAG_SECURE : 0);

    WinHttpSendRequest(
        state->hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        NULL,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        reinterpret_cast<DWORD_PTR>(state));
}

std::wstring HttpClient::ReadHeader(HINTERNET hRequest, DWORD dwHeader)
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

void HttpClient::StatusCallbackProxy(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpStatusInformation, DWORD dwStatusInformationLength)
{
    State* state = reinterpret_cast<State*>(dwContext);

    switch (dwInternetStatus)
    {
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    {
        state->dataSize = *((LPDWORD)lpStatusInformation);

        if (state->dataSize == 0)
        {
            HttpResponse response;
            response.body = state->response.str();
            response.statusCode = state->statusCode;

            state->callback(response);
            delete state;
        }
        else
        {
            char* buf = new char[state->dataSize];
            WinHttpReadData(
                state->hRequest,
                static_cast<LPVOID>(buf),
                state->dataSize,
                NULL);
        }
        break;
    }

    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
    {
        std::wstring status_str = ReadHeader(state->hRequest, WINHTTP_QUERY_STATUS_CODE);
        state->statusCode = std::stoi(status_str);

        WinHttpQueryDataAvailable(state->hRequest, NULL);
        break;
    }

    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    {
        if (dwStatusInformationLength != 0)
        {
            char* buf = static_cast<char*>(lpStatusInformation);
            state->response << std::string(buf, dwStatusInformationLength);
            delete[] buf;

            state->totalSize += state->dataSize;

            WinHttpQueryDataAvailable(state->hRequest, NULL);
        }
        break;
    }

    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
    {
        WinHttpReceiveResponse(state->hRequest, NULL);
        break;
    }
    }
}
