#include "generalpage.hpp"

#include "translator.hpp"

using pt::GeneralPage;

GeneralPage::GeneralPage(wxWindow* parent, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, wxID_ANY)
{
    wxStaticBoxSizer* uiSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "user_interface"));
    wxFlexGridSizer* uiGrid = new wxFlexGridSizer(2, 10, 10);
    m_language = new wxChoice(uiSizer->GetStaticBox(), wxID_ANY);
    uiGrid->AddGrowableCol(1, 1);
    uiGrid->Add(new wxStaticText(uiSizer->GetStaticBox(), wxID_ANY, i18n(tr, "language")), 0, wxALIGN_CENTER_VERTICAL);
    uiGrid->Add(m_language, 1, wxEXPAND);
    uiSizer->Add(uiGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* miscSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "miscellanneous"));
    wxFlexGridSizer* miscGrid = new wxFlexGridSizer(2, 10, 10);

    m_autoStart = new wxCheckBox(miscSizer->GetStaticBox(), wxID_ANY, i18n(tr, "start_with_windows"));
    m_startPosition = new wxChoice(miscSizer->GetStaticBox(), wxID_ANY);

    miscGrid->AddGrowableCol(1, 1);
    miscGrid->Add(m_autoStart, 0, wxALIGN_CENTER_VERTICAL);
    miscGrid->Add(0, 0);
    miscGrid->Add(new wxStaticText(miscSizer->GetStaticBox(), wxID_ANY, i18n(tr, "start_position")), 0, wxALIGN_CENTER_VERTICAL);
    miscGrid->Add(m_startPosition, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    miscSizer->Add(miscGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* notifSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "notification_area"));
    wxFlexGridSizer* notifGrid = new wxFlexGridSizer(1, 10, 10);

    m_showNotificationIcon = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n(tr, "show_picotorrent_in_notification_area"));
    m_minimizeNotification = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n(tr, "minimize_to_notification_area"));
    m_closeNotification = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n(tr, "close_to_notification_area"));

    notifGrid->AddGrowableCol(0, 1);
    notifGrid->Add(m_showNotificationIcon);
    notifGrid->Add(m_minimizeNotification);
    notifGrid->Add(m_closeNotification);
    notifSizer->Add(notifGrid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(uiSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(miscSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(notifSizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);
}

bool GeneralPage::Validate()
{
    if (!m_autoStart->IsChecked())
    {
        m_autoStart->SetFocus();
        return false;
    }

    return true;
}
