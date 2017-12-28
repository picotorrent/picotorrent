#include "preferencesdlg.hpp"

#include "config.hpp"
#include "connectionpage.hpp"
#include "downloadspage.hpp"
#include "generalpage.hpp"
#include "proxypage.hpp"
#include "translator.hpp"

#include <wx/bookctrl.h>

using pt::PreferencesDialog;

wxBEGIN_EVENT_TABLE(PreferencesDialog, wxPropertySheetDialog)
    EVT_BUTTON(wxID_OK, PreferencesDialog::OnOk)
wxEND_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(wxWindow* parent, std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Translator> tran)
{
    SetSheetStyle(wxPROPSHEET_LISTBOOK);

    Create(parent, wxID_ANY, i18n(tran, "preferences"));

    wxBookCtrlBase* book = GetBookCtrl();
    m_general = new GeneralPage(book, tran);
    m_downloads = new DownloadsPage(book, cfg, tran);

    book->AddPage(m_general, i18n(tran, "general"), true);
    book->AddPage(m_downloads, i18n(tran, "downloads"), false);
    book->AddPage(new ConnectionPage(book, tran), i18n(tran, "connection"), false);
    book->AddPage(new ProxyPage(book, tran), i18n(tran, "proxy"), false);

    CreateButtons();
    LayoutDialog();
}

void PreferencesDialog::OnOk(wxCommandEvent& event)
{
    if (!m_general->Validate())
    {
        // TODO: translate
        wxMessageBox("Invalid settings", "Validation error", 5L, this);
    }
    else
    {
        event.Skip();
    }
}
