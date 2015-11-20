#pragma once

#include <functional>
#include <string>
#include <windows.h>
#include <winhttp.h>

namespace picotorrent
{
namespace net
{
    class http_response;
    class uri;

    class http_client
    {
    public:
        http_client();
        ~http_client();

        void get_async(const uri &uri, const std::function<void(const http_response&)> &callback);
        std::string post(const std::wstring &url, const std::string &content);

    protected:
        static void CALLBACK callback_proxy(HINTERNET, DWORD_PTR, DWORD, LPVOID, DWORD);
        static std::wstring read_header(HINTERNET, DWORD);

    private:
        HINTERNET session_;
    };
}
}