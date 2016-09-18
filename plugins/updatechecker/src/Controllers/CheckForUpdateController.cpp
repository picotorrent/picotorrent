#include "CheckForUpdateController.hpp"

#include <picojson.hpp>
#include <picotorrent/api.hpp>
#include <semver.hpp>

#include "../Dialogs/NoUpdateAvailableDialog.hpp"
#include "../Dialogs/UpdateAvailableDialog.hpp"
#include "../Net/HttpClient.hpp"
#include "../Net/HttpResponse.hpp"

namespace pj = picojson;
using Controllers::CheckForUpdateController;

CheckForUpdateController::CheckForUpdateController(std::shared_ptr<IPicoTorrent> pico)
    : m_httpClient(std::make_unique<Net::HttpClient>()),
    m_pico(pico)
{
}

void CheckForUpdateController::Execute(bool forced)
{
    const std::wstring url = L"https://api.github.com/repos/picotorrent/picotorrent/releases/latest";
    m_httpClient->GetAsync(url, std::bind(&CheckForUpdateController::OnHttpResponse, this, std::placeholders::_1, forced));
}

void CheckForUpdateController::OnHttpResponse(Net::HttpResponse httpResponse, bool forced)
{
    if (httpResponse.statusCode != 200
        && httpResponse.statusCode != 202)
    {
        // LOG
        return;
    }

    pj::value v;
    std::string err = pj::parse(v, httpResponse.body);

    if (!err.empty())
    {
        // LOG
        return;
    }

    picojson::object obj = v.get<picojson::object>();
    std::string version = obj["tag_name"].get<std::string>();
    if (version[0] == 'v') { version = version.substr(1); }

    // If we haven't forced an update (via the menu item)
    // and the version is the same as the stored ignored version,
    // just return silently.

    if (version == "ignored version" && !forced)
    {
        return;
    }

    semver::version parsedVersion(version);
    semver::version currentVersion(m_pico->GetCurrentVersion());

    if (parsedVersion > currentVersion)
    {
        Dialogs::UpdateAvailableDialog dlg(m_pico);
        dlg.Show();
    }
    else if (forced)
    {
        Dialogs::NoUpdateAvailableDialog dlg(m_pico);
        dlg.Show();
    }
}
