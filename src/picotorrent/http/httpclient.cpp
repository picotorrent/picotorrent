#include "httpclient.hpp"

#include <sstream>

wxDEFINE_EVENT(ptEVT_HTTP_RESPONSE, wxCommandEvent);

struct State
{
    ~State()
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
    }

    pt::Http::HttpClient* client;
    std::function<void(int, std::string const&)> callback;
    HINTERNET hConnect;
    HINTERNET hRequest;
    std::stringstream response;
    DWORD dataSize = 0;
    DWORD totalSize = 0;
    int statusCode = 0;
};

using pt::Http::HttpClient;

HttpClient::HttpClient()
{
    m_session = WinHttpOpen(
        L"PicoTorrent/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC);

    WinHttpSetStatusCallback(
        m_session,
        &HttpClient::StatusCallbackProxy,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS,
        NULL);

    this->Bind(
        ptEVT_HTTP_RESPONSE,
        [](wxCommandEvent const& evt)
        {
            auto state = reinterpret_cast<State*>(evt.GetClientData());
            state->callback(state->statusCode, state->response.str());
            delete state;
        });
}

HttpClient::~HttpClient()
{
    WinHttpCloseHandle(m_session);
}

void HttpClient::Get(wxString const& url, std::function<void(int, std::string const&)> const& callback)
{
    // Crack URI
    URL_COMPONENTS uc = { sizeof(URL_COMPONENTS) };
    uc.dwSchemeLength = DWORD(-1);
    uc.dwHostNameLength = DWORD(-1);
    uc.dwUrlPathLength = DWORD(-1);
    uc.dwExtraInfoLength = DWORD(-1);
    WinHttpCrackUrl(url.wc_str(), static_cast<DWORD>(url.size()), 0, &uc);

    std::wstring scheme(uc.lpszScheme, uc.dwSchemeLength);
    bool secure = scheme == L"https";

    std::wstring host(uc.lpszHostName, uc.dwHostNameLength);
    HINTERNET hConnect = WinHttpConnect(m_session, host.c_str(), uc.nPort, NULL);
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", uc.lpszUrlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, secure ? WINHTTP_FLAG_SECURE : 0);

    auto state = new State();
    state->callback = callback;
    state->client = this;
    state->hConnect = hConnect;
    state->hRequest = hRequest;

    WinHttpSendRequest(
        hRequest,
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

void HttpClient::StatusCallbackProxy(HINTERNET, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpStatusInformation, DWORD dwStatusInformationLength)
{
    State* state = reinterpret_cast<State*>(dwContext);

    switch (dwInternetStatus)
    {
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    {
        state->dataSize = *((LPDWORD)lpStatusInformation);

        if (state->dataSize == 0)
        {
            wxCommandEvent evt(ptEVT_HTTP_RESPONSE);
            evt.SetClientData(state);
            wxPostEvent(state->client, evt);
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
