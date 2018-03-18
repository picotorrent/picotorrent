#pragma once

#include <functional>
#include <string>

#include <windows.h>
#include <winhttp.h>

namespace pt
{
    namespace http
    {
        struct HttpResponse
        {
            std::string body;
            int statusCode;
        };

        class HttpClient
        {
        public:
            HttpClient();
            ~HttpClient();

            void GetAsync(const std::wstring& url, std::function<void(HttpResponse)> const& callback);

        private:
            static std::wstring ReadHeader(HINTERNET hRequest, DWORD dwHeader);
            static void CALLBACK StatusCallbackProxy(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpStatusInformation, DWORD dwStatusInformationLength);

            HINTERNET m_hSession;
        };
    }
}
