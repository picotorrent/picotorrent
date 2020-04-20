#include "updatechecker.hpp"

#include <Windows.h>
#include <CommCtrl.h>

#include <loguru.hpp>

#include "buildinfo.hpp"
#include "core/configuration.hpp"
#include "http/httpclient.hpp"
#include "picojson.hpp"
#include "semver.hpp"
#include "ui/translator.hpp"

using pt::UpdateChecker;

UpdateChecker::UpdateChecker(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg)
    : m_parent(parent),
    m_cfg(cfg)
{
}

UpdateChecker::~UpdateChecker()
{
}

void UpdateChecker::Check(bool force)
{
    std::string url = m_cfg->GetString("update_checks.url");

    if (url.empty())
    {
        LOG_F(WARNING, "No update check URL set. Skipping...");
        return;
    }

    auto client = std::make_shared<Http::HttpClient>();
    client->Get(url,
        [this, client, force](int statusCode, std::string const& body)
        {
            if (statusCode != 200)
            {
                LOG_F(ERROR, "HTTP response status not 200, was %d", statusCode);
                return;
            }

            picojson::value res;
            std::string err = picojson::parse(res, body);

            if (!err.empty())
            {
                LOG_F(ERROR, "Failed to parse release JSON: %s", err.data());
                return;
            }

            picojson::object obj = res.get<picojson::object>();

            std::string version = obj["version"].get<std::string>();
            std::string ignoredVersion = m_cfg->GetString("update_checks.ignored_version");

            // If we haven't forced an update (via the menu item)
            // and the version is the same as the stored ignored version,
            // just return silently.

            if (version == ignoredVersion && !force)
            {
                return;
            }

            semver::version parsedVersion(version);
            semver::version currentVersion(BuildInfo::version());

            if (parsedVersion > currentVersion)
            {
                // Show update available
                this->ShowUpdateDialog(
                    version,
                    obj["url"].get<std::string>());
            }
            else if (force)
            {
                this->ShowNoUpdateDialog();
            }

            wxCommandEvent evt;
            wxPostEvent(this, evt);
        });
}

void UpdateChecker::ShowNoUpdateDialog()
{
    // Load translations
    wxString main = i18n("no_update_available");

    TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
    tdf.dwCommonButtons = TDCBF_OK_BUTTON;
    tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW;
    tdf.hwndParent = m_parent->GetHWND();
    tdf.pszMainIcon = TD_INFORMATION_ICON;
    tdf.pszMainInstruction = main.c_str();
    tdf.pszWindowTitle = TEXT("PicoTorrent");

    TaskDialogIndirect(&tdf, nullptr, nullptr, nullptr);
}

void UpdateChecker::ShowUpdateDialog(std::string const& version, std::string& url)
{
    // Load translations
    wxString content = i18n("new_version_available");
    wxString main = i18n("picotorrent_v_available");
    wxString mainFormatted = wxString::Format(main, version);
    wxString verification = i18n("ignore_update");
    wxString show = i18n("show_on_github");

    const TASKDIALOG_BUTTON pButtons[] =
    {
        { 1000, show.c_str() },
    };

    TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
    tdf.cButtons = ARRAYSIZE(pButtons);
    tdf.dwCommonButtons = TDCBF_CLOSE_BUTTON;
    tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW | TDF_USE_COMMAND_LINKS;
    tdf.hwndParent = m_parent->GetHWND();
    tdf.pButtons = pButtons;
    tdf.pszMainIcon = TD_INFORMATION_ICON;
    tdf.pszMainInstruction = mainFormatted;
    tdf.pszVerificationText = verification.c_str();
    tdf.pszWindowTitle = TEXT("PicoTorrent");

    int pnButton = -1;
    int pnRadioButton = -1;
    BOOL pfVerificationFlagChecked = FALSE;

    TaskDialogIndirect(&tdf, &pnButton, &pnRadioButton, &pfVerificationFlagChecked);

    if (pnButton == 1000)
    {
        wxLaunchDefaultBrowser(url, wxBROWSER_NEW_WINDOW);
    }

    if (pfVerificationFlagChecked)
    {
        m_cfg->SetString("update_checks.ignored_version", version);
    }
}
