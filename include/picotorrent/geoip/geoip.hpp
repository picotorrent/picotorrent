#pragma once

#include <memory>
#include <string>

#include <picotorrent/export.hpp>

#include <QObject>

namespace pt
{
    class Configuration;
    class Environment;
    class HttpClient;
    class HttpResponse;

    class PICO_GEOIP_API GeoIP : public QObject
    {
        Q_OBJECT

    public:
        GeoIP(QObject* parent, std::shared_ptr<Environment> env, std::shared_ptr<Configuration> cfg);
        ~GeoIP();

        void load();
        std::string lookupCountryCode(std::string const& ip);

    public slots:
        /*
        Updates the MaxMind GeoIP database. Emits the `databaseUpdated`
        signal when the new database has been downloaded and loaded in
        memory.
        */
        void update();

    signals:
        void databaseLoaded();
        void updateRequired();

    private slots:
        void databaseDownloaded(HttpResponse* response);

    private:
        struct DatabaseHandle;

        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Environment> m_env;
        std::shared_ptr<HttpClient> m_httpClient;
        std::shared_ptr<DatabaseHandle> m_db;
    };
}
