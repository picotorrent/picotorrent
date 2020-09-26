#include "preferencesdialog.hpp"

#include <CommCtrl.h>

#include <wx/bookctrl.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/hyperlink.h>
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/simplebook.h>

#include "../../core/configuration.hpp"
#include "preferencesadvancedpage.hpp"
#include "preferencesconnectionpage.hpp"
#include "preferencesdownloadspage.hpp"
#include "preferencesgeneralpage.hpp"
#include "preferencesproxypage.hpp"
#include "../translator.hpp"

using pt::UI::Dialogs::PreferencesDialog;

PreferencesDialog::PreferencesDialog(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg)
    : wxDialog(parent, wxID_ANY, i18n("preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    m_book(new wxSimplebook(this, wxID_ANY)),
    m_general(new PreferencesGeneralPage(m_book, cfg)),
    m_downloads(new PreferencesDownloadsPage(m_book, cfg)),
    m_connection(new PreferencesConnectionPage(m_book, cfg)),
    m_proxy(new PreferencesProxyPage(m_book, cfg)),
    m_advanced(new PreferencesAdvancedPage(m_book, cfg)),
    m_wantsRestart(false)
{
    m_list = new wxListBox(this, wxID_ANY);
    m_list->Append(i18n("general"));
    m_list->Append(i18n("downloads"));
    m_list->Append(i18n("connection"));
    m_list->Append(i18n("proxy"));
    m_list->Append(i18n("advanced"));
    m_list->Select(0);

    m_book->AddPage(m_general, wxEmptyString, true);
    m_book->AddPage(m_downloads, wxEmptyString, false);
    m_book->AddPage(m_connection, wxEmptyString, false);
    m_book->AddPage(m_proxy, wxEmptyString, false);
    m_book->AddPage(m_advanced, wxEmptyString, false);

    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainSizer->Add(m_list, 0, wxEXPAND | wxRIGHT, FromDIP(7));
    m_mainSizer->Add(m_book, 1, wxEXPAND);

    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxHyperlinkCtrl(this, wxID_ANY, i18n("documentation"), "https://docs.picotorrent.org/en/master/configuration.html"), 0, wxALIGN_CENTER_VERTICAL);
    buttonSizer->AddStretchSpacer(1);
    buttonSizer->Add(new wxButton(this, ptID_BTN_RESTORE_DEFAULTS, i18n("restore_defaults")), 0, wxRIGHT, FromDIP(7));
    buttonSizer->Add(new wxButton(this, wxID_OK, i18n("ok")), 0, wxRIGHT, FromDIP(7));
    buttonSizer->Add(new wxButton(this, wxID_CANCEL, i18n("cancel")));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_mainSizer, 1, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, FromDIP(11));
    sizer->AddSpacer(FromDIP(7));
    sizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, FromDIP(11));

    this->SetSizerAndFit(sizer);

    if (!wxPersistenceManager::Get().RegisterAndRestore(this))
    {
        this->SetSize(FromDIP(wxSize(450, 400)));
    }

    this->Bind(wxEVT_BUTTON, &PreferencesDialog::OnOk, this, wxID_OK);
    this->Bind(
        wxEVT_LISTBOX,
        [this](wxCommandEvent&)
        {
            m_book->ChangeSelection(
                m_list->GetSelection());
        });

    this->Bind(
        wxEVT_BUTTON,
        [this, cfg](wxCommandEvent&)
        {
            if (wxMessageBox(
                    i18n("restore_defaults_description"),
                    "PicoTorrent",
                    wxICON_WARNING | wxYES_NO | wxNO_DEFAULT) == wxYES)
            {
                cfg->RestoreDefaults();
                this->EndDialog(wxID_OK);
            }
        },
        ptID_BTN_RESTORE_DEFAULTS);
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::OnOk(wxCommandEvent& evt)
{
    if (!m_general->IsValid())
    {
        return;
    }

    if (!m_downloads->IsValid())
    {
        return;
    }

    if (!m_connection->IsValid())
    {
        return;
    }

    if (!m_proxy->IsValid())
    {
        return;
    }

    if (!m_advanced->IsValid())
    {
        return;
    }

    bool restartRequired = false;

    m_general->Save(&restartRequired);
    m_downloads->Save();
    m_connection->Save(&restartRequired);
    m_proxy->Save();
    m_advanced->Save();

    if (restartRequired)
    {
        ShowRestartRequiredDialog();
    }

    evt.Skip();
}

void PreferencesDialog::ShowRestartRequiredDialog()
{
    std::wstring btn = i18n("restart_picotorrent");
    std::wstring main = i18n("prompt_restart");
    std::wstring title = i18n("prompt_restart_title");

    const TASKDIALOG_BUTTON pButtons[] =
    {
        { 1000, btn.c_str() },
    };

    TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
    tdf.cButtons = ARRAYSIZE(pButtons);
    tdf.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW;
    tdf.hwndParent = GetHWND();
    tdf.pButtons = pButtons;
    tdf.pszMainIcon = TD_INFORMATION_ICON;
    tdf.pszMainInstruction = main.c_str();
    tdf.pszWindowTitle = title.c_str();

    int pnButton = -1;
    int pnRadioButton = -1;
    BOOL pfVerificationFlagChecked = FALSE;

    TaskDialogIndirect(&tdf, &pnButton, &pnRadioButton, &pfVerificationFlagChecked);

    m_wantsRestart = (pnButton == 1000);
}
