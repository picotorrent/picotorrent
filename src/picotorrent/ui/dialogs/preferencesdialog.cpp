#include "preferencesdialog.hpp"

#include <wx/bookctrl.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/persist.h>
#include <wx/persist/toplevel.h>

#include "../../core/configuration.hpp"
#include "preferencesconnectionpage.hpp"
#include "preferencesdownloadspage.hpp"
#include "preferencesgeneralpage.hpp"
#include "preferencesproxypage.hpp"
#include "../translator.hpp"

using pt::UI::Dialogs::PreferencesDialog;

PreferencesDialog::PreferencesDialog(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg)
{
    SetName("PreferencesDialog");
    SetSheetStyle(wxPROPSHEET_LISTBOOK);

    Create(
        parent,
        wxID_ANY,
        i18n("preferences"),
        wxDefaultPosition,
        wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    wxBookCtrlBase* book = GetBookCtrl();
    m_general = new PreferencesGeneralPage(book, cfg);
    m_downloads = new PreferencesDownloadsPage(book, cfg);
    m_connection = new PreferencesConnectionPage(book, cfg);
    m_proxy = new PreferencesProxyPage(book, cfg);
    /*m_advanced = new AdvancedPage(book, cfg, tran);*/

    book->AddPage(m_general, i18n("general"), true);
    book->AddPage(m_downloads, i18n("downloads"));
    book->AddPage(m_connection, i18n("connection"));
    book->AddPage(m_proxy, i18n("proxy"));

    /*book->AddPage(m_advanced, i18n("advanced"), false);*/

    CreateButtons();
    LayoutDialog();

    wxPersistenceManager::Get().RegisterAndRestore(this);

    this->Bind(wxEVT_BUTTON, &PreferencesDialog::OnOk, this, wxID_OK);
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

    m_general->Save();
    m_downloads->Save();
    m_connection->Save();
    m_proxy->Save();

    evt.Skip();
}
