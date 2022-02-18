#include "httpclient.hpp"
#include "httprequest.hpp"
#include "httpresponse.hpp"
/*
#include <Windows.h>
#include <winhttp.h>
*/
using pt::HttpClient;

struct HttpClient::InternetHandle
{
    HINTERNET hInternet;
};

struct State
{
    HINTERNET hConnect;
    HINTERNET hRequest;

    pt::HttpResponse* response;

    DWORD dataSize = 0;
    DWORD totalSize = 0;
};

static std::wstring ReadHeader(HINTERNET hRequest, DWORD dwHeader)
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

static void CALLBACK StatusCallbackProxy(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpStatusInformation, DWORD dwStatusInformationLength)
{
    State* state = reinterpret_cast<State*>(dwContext);

    switch (dwInternetStatus)
    {
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    {
        state->dataSize = *((LPDWORD)lpStatusInformation);

        if (state->dataSize == 0)
        {
            emit state->response->finished(state->response);
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
        state->response->statusCode = std::stoi(status_str);

        WinHttpQueryDataAvailable(state->hRequest, NULL);
        break;
    }

    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    {
        if (dwStatusInformationLength != 0)
        {
            char* buf = static_cast<char*>(lpStatusInformation);

            state->response->body.insert(
                state->response->body.end(),
                buf,
                buf + dwStatusInformationLength);

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

HttpClient::HttpClient(QString const& userAgent)
{
    m_handle = new InternetHandle();
    m_handle->hInternet = WinHttpOpen(
        userAgent.toStdWString().c_str(),
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC);

    WinHttpSetStatusCallback(
        m_handle->hInternet,
        &StatusCallbackProxy,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS,
        NULL);
}

HttpClient::~HttpClient()
{
    WinHttpCloseHandle(m_handle->hInternet);
    delete m_handle;
}

pt::HttpResponse* HttpClient::get(pt::HttpRequest const& req)
{
    QUrl    url    = req.url();
    QString host   = url.host();
    QString path   = url.path();
    QString scheme = url.scheme();

    bool secure = scheme == "https";
    int port = url.port(secure ? 443 : 80);

    State* state = new State();
    state->response = new HttpResponse();

    state->hConnect = WinHttpConnect(
        m_handle->hInternet,
        host.toStdWString().c_str(),
        port,
        NULL);

    state->hRequest = WinHttpOpenRequest(
        state->hConnect,
        L"GET",
        path.toStdWString().c_str(),
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        secure ? WINHTTP_FLAG_SECURE : 0);

    WinHttpSendRequest(
        state->hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        NULL,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        reinterpret_cast<DWORD_PTR>(state));

    return state->response;
}
