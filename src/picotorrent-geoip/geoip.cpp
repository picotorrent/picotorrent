#include <picotorrent/geoip/geoip.hpp>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

#include <maxminddb.h>

#include <picotorrent/core/configuration.hpp>
#include <picotorrent/core/environment.hpp>
#include <picotorrent/http/httpclient.hpp>
#include <picotorrent/http/httprequest.hpp>
#include <picotorrent/http/httpresponse.hpp>

#include "gzipdecompressor.hpp"
#include "maxminddatabase.hpp"

namespace fs = std::experimental::filesystem;
using pt::GeoIP;

struct GeoIP::DatabaseHandle : public pt::MaxMindDatabase
{
};

static int OneMonth = 60 * 60 * 24 * 30;

GeoIP::GeoIP(QObject* parent, std::shared_ptr<pt::Environment> env, std::shared_ptr<pt::Configuration> cfg)
    : QObject(parent),
    m_env(env),
    m_cfg(cfg)
{
    m_db = std::make_shared<DatabaseHandle>();
    m_httpClient = std::make_shared<pt::HttpClient>("PicoTorrent-GeoIPManager/1.0");
}

GeoIP::~GeoIP()
{
}

std::string GeoIP::lookupCountryCode(std::string const& ip)
{
    return m_db->lookup(ip);
}

void GeoIP::load()
{
    fs::path db = m_env->getApplicationDataPath() / "GeoLite2-Country.mmdb";

    if (!fs::exists(db))
    {
        emit updateRequired();
        return;
    }

    auto ftime = fs::last_write_time(db);
    auto now = std::chrono::system_clock::now();

    std::chrono::duration<double> age = now - ftime;

    if (age.count() >= OneMonth)
    {
        emit updateRequired();
        return;
    }

    m_db->load(db);

    emit databaseLoaded();
}

void GeoIP::update()
{
    QString databaseUrl = QString::fromStdString(m_cfg->getString("geoip.database_url"));
    QUrl url(databaseUrl);

    HttpRequest req(url);
    HttpResponse* res = m_httpClient->get(req);

    QObject::connect(res,  &HttpResponse::finished,
                     this, &GeoIP::databaseDownloaded);
}

void GeoIP::databaseDownloaded(pt::HttpResponse* response)
{
    GZipDecompressor gzip;
    auto decompressed = gzip.decompress(response->body);

    // Save file on disk
    fs::path db = m_env->getApplicationDataPath() / "GeoLite2-Country.mmdb";
    
    std::ofstream out(db, std::ios::binary);
    std::copy(
        decompressed.begin(),
        decompressed.end(),
        std::ostreambuf_iterator<char>(out));
    out.close();

    // Reload the database
    m_db->load(db);

    emit databaseLoaded();

    response->deleteLater();
}
