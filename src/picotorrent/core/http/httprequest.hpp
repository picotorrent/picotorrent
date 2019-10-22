#pragma once

#include <QUrl>

namespace pt
{
    class HttpResponse;

    class HttpRequest
    {
    public:
        HttpRequest(QUrl const& url);
        ~HttpRequest();

        QUrl& url() const;

    private:
        QUrl* m_url;
    };
}
