#include "updatechecker.hpp"

#include <QMessageBox>

#include <picotorrent/core/configuration.hpp>
#include <picotorrent/http/httpclient.hpp>
#include <picotorrent/http/httprequest.hpp>
#include <picotorrent/http/httpresponse.hpp>

#include "buildinfo.hpp"
#include "picojson.hpp"
#include "semver.hpp"
#include "translator.hpp"
#include "updateinformation.hpp"

using pt::UpdateChecker;

UpdateChecker::UpdateChecker(std::shared_ptr<pt::Configuration> cfg, bool forced)
    : m_cfg(cfg),
    m_forced(forced)
{
}

UpdateChecker::~UpdateChecker()
{
}

void UpdateChecker::check()
{
    QString url = QString::fromStdString(m_cfg->getString("update_checks.url"));
    HttpRequest req(url);

    auto httpClient = new HttpClient("PicoTorrent/" + BuildInfo::version());
    auto response   = httpClient->get(req);

    QObject::connect(response,   &HttpResponse::finished,
                     this,       &UpdateChecker::parseResponse);

    QObject::connect(response,   &HttpResponse::finished,
                     response,   &HttpResponse::deleteLater);

    QObject::connect(response,   &HttpResponse::finished,
                     httpClient, &HttpClient::deleteLater);
}

void UpdateChecker::parseResponse(pt::HttpResponse* response)
{
    if (response->statusCode != 200)
    {
        return;
    }

    picojson::value res;
    std::string err;
    picojson::parse(res, response->body.begin(), response->body.end(), &err);

    if (!err.empty())
    {
        return;
    }

    picojson::object obj = res.get<picojson::object>();

    std::string version = obj["version"].get<std::string>();
    std::string ignoredVersion = m_cfg->getString("update_checks.ignored_version");

    // If we haven't forced an update (via the menu item)
    // and the version is the same as the stored ignored version,
    // just return silently.

    if (version == ignoredVersion && !m_forced)
    {
        return;
    }

    semver::version parsedVersion(version);
    semver::version currentVersion(BuildInfo::version().toStdString());

    if (parsedVersion > currentVersion)
    {
        UpdateInformation info;
        info.available = true;
        info.version = QString::fromStdString(version);
        info.url = QString::fromStdString(obj["url"].get<std::string>());

        emit finished(&info);
    }
    else if (m_forced)
    {
        UpdateInformation info = { 0 };
        info.available = false;

        emit finished(&info);
    }
}