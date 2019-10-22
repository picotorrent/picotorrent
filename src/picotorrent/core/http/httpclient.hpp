#pragma once

#include <QObject>
#include <QString>

namespace pt
{
    class HttpRequest;
    class HttpResponse;

    class HttpClient : public QObject
    {
        Q_OBJECT

    public:
        HttpClient(QString const& userAgent);
        ~HttpClient();

        HttpResponse* get(HttpRequest const& req);

    private:
        struct InternetHandle;
        InternetHandle* m_handle;
    };
}
