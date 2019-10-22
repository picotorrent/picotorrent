#pragma once

#include <memory>

#include <QObject>

class QWidget;

namespace pt
{
    class Configuration;
    class HttpResponse;
    struct UpdateInformation;

    class UpdateChecker : public QObject
    {
        Q_OBJECT

    public:
        UpdateChecker(std::shared_ptr<Configuration> cfg, bool forced = false);
        ~UpdateChecker();

        void check();

    signals:
        void finished(UpdateInformation* info);

    private slots:
        void parseResponse(HttpResponse* response);

    private:
        std::shared_ptr<Configuration> m_cfg;
        bool m_forced;
    };
}
