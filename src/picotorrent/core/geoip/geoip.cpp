#include "geoip.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

#include <maxminddb.h>

#include "../configuration.hpp"
#include "../environment.hpp"
#include "../http/httpclient.hpp"
#include "../http/httprequest.hpp"
#include "../http/httpresponse.hpp"

#include "loguru.hpp"

#include "gzipdecompressor.hpp"
#include "maxminddatabase.hpp"

namespace fs = std::filesystem;
using pt::GeoIP;

struct GeoIP::DatabaseHandle : public pt::MaxMindDatabase
{
};

static int HoursOneMonth = 24 * 30;

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
        LOG_F(INFO, "GeoIP database not found on disk. Updating...");
        emit updateRequired();
        return;
    }

    auto ftime = fs::last_write_time(db);
	auto now = fs::file_time_type::clock::now();
	auto hours = std::chrono::duration_cast<std::chrono::hours>(now - ftime);

    std::chrono::duration<double> age = now - ftime;

    if (age.count() >= HoursOneMonth)
    {
        LOG_F(INFO, "GeoIP database more than one month old. Updating...");
        emit updateRequired();
        return;
    }

    LOG_F(INFO, "Loading GeoIP database");

    m_db->load(db);

    emit databaseLoaded();
}

void GeoIP::update()
{
    QString databaseUrl = QString::fromStdString(m_cfg->getString("geoip.database_url"));
    QUrl url(databaseUrl);

    LOG_F(INFO, "Updating GeoIP database from %s", databaseUrl.toStdString().data());

    HttpRequest req(url);
    HttpResponse* res = m_httpClient->get(req);

    QObject::connect(res,  &HttpResponse::finished,
                     this, &GeoIP::databaseDownloaded);
}

void GeoIP::databaseDownloaded(pt::HttpResponse* response)
{
    LOG_F(INFO, "GeoIP database downloaded... Decompressing");

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

    LOG_F(INFO, "GeoIP database saved. Loading...");

    // Reload the database
    m_db->load(db);

    emit databaseLoaded();

    response->deleteLater();
}
