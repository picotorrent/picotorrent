#include "preferencesdlg.hpp"

#include "config.hpp"
#include "connectionpage.hpp"
#include "downloadspage.hpp"
#include "environment.hpp"
#include "generalpage.hpp"
#include "proxypage.hpp"
#include "translator.hpp"

#include <wx/bookctrl.h>
#include <wx/listbook.h>

using pt::PreferencesDialog;

wxBEGIN_EVENT_TABLE(PreferencesDialog, wxPropertySheetDialog)
    EVT_BUTTON(wxID_OK, PreferencesDialog::OnOk)
wxEND_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(wxWindow* parent, std::shared_ptr<pt::Environment> env, std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Translator> tran)
    : m_cfg(cfg),
    m_env(env)
{
    SetSheetStyle(wxPROPSHEET_LISTBOOK);

    Create(parent, wxID_ANY, i18n(tran, "preferences"));

    wxBookCtrlBase* book = GetBookCtrl();
    m_general = new GeneralPage(book, cfg, tran);
    m_downloads = new DownloadsPage(book, cfg, tran);
    m_connection = new ConnectionPage(book, cfg, tran);
    m_proxy = new ProxyPage(book, cfg, tran);

    book->AddPage(m_general, i18n(tran, "general"), true);
    book->AddPage(m_downloads, i18n(tran, "downloads"), false);
    book->AddPage(m_connection, i18n(tran, "connection"), false);
    book->AddPage(m_proxy, i18n(tran, "proxy"), false);

    CreateButtons();
    LayoutDialog();
}

void PreferencesDialog::OnOk(wxCommandEvent& event)
{
    wxString error;

    if (!m_general->ValidateConfiguration(error))
    {
        GetBookCtrl()->SetSelection(0);
        wxMessageBox("Invalid settings", error, 5L, this); // TODO: translate
    }
    else if (!m_downloads->ValidateConfiguration(error))
    {
        GetBookCtrl()->SetSelection(1);
        wxMessageBox("Invalid settings", error, 5L, this); // TODO: translate
    }
    else if (!m_connection->ValidateConfiguration(error))
    {
        GetBookCtrl()->SetSelection(2);
        wxMessageBox("Invalid settings", error, 5L, this); // TODO: translate
    }
    else if (!m_proxy->ValidateConfiguration(error))
    {
        GetBookCtrl()->SetSelection(3);
        wxMessageBox("Invalid settings", error, 5L, this); // TODO: translate
    }
    else
    {
        m_general->ApplyConfiguration();
        m_downloads->ApplyConfiguration();
        m_connection->ApplyConfiguration();
        m_proxy->ApplyConfiguration();

        Configuration::Save(m_env, m_cfg);

        event.Skip();
    }
}
