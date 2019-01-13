#pragma once

#include <picotorrent/export.hpp>
#include <QObject>

#include <stdint.h>
#include <vector>

namespace pt
{
    class PICO_HTTP_API HttpResponse : public QObject
    {
        Q_OBJECT

    public:
        HttpResponse();
        virtual ~HttpResponse();

        int statusCode;
        std::vector<char> body;

    signals:
        void error();
        void finished(HttpResponse* response);
        void progress(int64_t current, int64_t total);
    };
}
