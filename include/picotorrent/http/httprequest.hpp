#pragma once

#include <picotorrent/export.hpp>

#include <QUrl>

namespace pt
{
    class HttpResponse;

    class PICO_HTTP_API HttpRequest
    {
    public:
        HttpRequest(QUrl const& url);
        ~HttpRequest();

        QUrl& url() const;

    private:
        QUrl* m_url;
    };
}
