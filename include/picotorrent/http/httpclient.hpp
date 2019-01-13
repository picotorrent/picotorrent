#pragma once

#include <QString>

#include <picotorrent/export.hpp>

namespace pt
{
    class HttpRequest;
    class HttpResponse;

    class PICO_HTTP_API HttpClient
    {
    public:
        HttpClient(QString const& userAgent);
        ~HttpClient();

        HttpResponse* get(HttpRequest const& req);

    private:
        struct InternetHandle;
        InternetHandle* m_handle;
    };
}
