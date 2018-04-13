#include "applicationupdater.hpp"

#include <wx/richmsgdlg.h>

#include "buildinfo.hpp"
#include "config.hpp"
#include "translator.hpp"
#include "http/httpclient.hpp"
#include "picojson.hpp"
#include "semver.hpp"
#include "utils.hpp"

using pt::ApplicationUpdater;

ApplicationUpdater::ApplicationUpdater(wxFrame* parent, std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Translator> translator)
    : m_httpClient(std::make_unique<http::HttpClient>()),
    m_config(cfg),
    m_translator(translator),
    m_parent(parent)
{
}

ApplicationUpdater::~ApplicationUpdater()
{
}

void ApplicationUpdater::Check(bool force)
{
    wxString updateUrl = m_config->UpdateUrl();

    m_httpClient->GetAsync(
        updateUrl.ToStdWstring(),
        std::bind(&ApplicationUpdater::OnHttpResponse, this, std::placeholders::_1, force));
}

void ApplicationUpdater::OnHttpResponse(pt::http::HttpResponse const& response, bool force)
{
    if (response.statusCode != 200)
    {
        return;
    }

    picojson::value v;
    std::string err = picojson::parse(v, response.body);

    if (!err.empty())
    {
        return;
    }

    picojson::object obj = v.get<picojson::object>();
    std::string version = obj["version"].get<std::string>();

    // If we haven't forced an update (via the menu item)
    // and the version is the same as the stored ignored version,
    // just return silently.

    if (version == m_config->IgnoredVersion() && !force)
    {
        return;
    }

    semver::version parsedVersion(version);
    semver::version currentVersion(BuildInfo::Version());

    if (parsedVersion > currentVersion)
    {
        std::string url = obj["url"].get<std::string>();

        m_parent->GetEventHandler()->CallAfter(std::bind(&ApplicationUpdater::ShowUpdateDialog, this, version, url));
    }
    else if (force)
    {
        m_parent->GetEventHandler()->CallAfter(std::bind(&ApplicationUpdater::ShowNoUpdateDialog, this));
    }
}

void ApplicationUpdater::ShowUpdateDialog(std::string const& version, std::string& url)
{
    // Load translations
    wxString content = i18n(m_translator, "new_version_available");
    wxString main = i18n(m_translator, "picotorrent_v_available");
    wxString mainFormatted = wxString::Format(main, version);
    wxString verification = i18n(m_translator, "ignore_update");
    wxString show = i18n(m_translator, "show_on_github");

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
        m_config->IgnoredVersion(version);
    }
}

void ApplicationUpdater::ShowNoUpdateDialog()
{
    // Load translations
    wxString main = i18n(m_translator, "no_update_available");

    TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
    tdf.dwCommonButtons = TDCBF_OK_BUTTON;
    tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW;
    tdf.hwndParent = m_parent->GetHWND();
    tdf.pszMainIcon = TD_INFORMATION_ICON;
    tdf.pszMainInstruction = main.c_str();
    tdf.pszWindowTitle = TEXT("PicoTorrent");

    TaskDialogIndirect(&tdf, nullptr, nullptr, nullptr);
}
