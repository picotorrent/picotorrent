#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <Windows.h>
#include <winhttp.h>

#include <memory>

namespace pt
{
namespace Http
{
    struct HttpResponse
    {
        std::string body;
        int statusCode;
    };

    class HttpClient : public wxEvtHandler
    {
    public:
        HttpClient();
        virtual ~HttpClient();

        void Get(wxString const& url, std::function<void(int, std::string const&)> const& callback);
    private:
        static std::wstring ReadHeader(HINTERNET hRequest, DWORD dwHeader);
        static void CALLBACK StatusCallbackProxy(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpStatusInformation, DWORD dwStatusInformationLength);

        HINTERNET m_session;
    };
}
}
