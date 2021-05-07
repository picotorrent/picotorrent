#include "preferencesdownloadspage.hpp"

#include "../../core/configuration.hpp"
#include "../../core/utils.hpp"
#include "../translator.hpp"

#include <wx/filepicker.h>
#include <wx/statline.h>

using pt::UI::Dialogs::PreferencesDownloadsPage;

PreferencesDownloadsPage::PreferencesDownloadsPage(wxWindow* parent, std::shared_ptr<pt::Core::Configuration> cfg)
    : wxPanel(parent, wxID_ANY),
    m_cfg(cfg)
{
    wxStaticBoxSizer* transfersSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("transfers"));
    wxFlexGridSizer* transfersGrid = new wxFlexGridSizer(2, 10, 10);

    m_savePathCtrl = new wxDirPickerCtrl(transfersSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);
    m_savePathCtrl->SetPath(wxString::FromUTF8(m_cfg->Get<std::string>("default_save_path").value()));

    m_moveCompletedEnabled = new wxCheckBox(transfersSizer->GetStaticBox(), wxID_ANY, i18n("move_completed_downloads"));
    m_moveCompletedPathCtrl = new wxDirPickerCtrl(transfersSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);
    m_moveCompletedOnlyFromDefault = new wxCheckBox(transfersSizer->GetStaticBox(), wxID_ANY, i18n("only_move_from_default_save_path"));
    m_pauseLowDiskSpace = new wxCheckBox(transfersSizer->GetStaticBox(), wxID_ANY, i18n("pause_on_low_disk_space"));
    m_pauseLowDiskSpace->SetValue(cfg->Get<bool>("pause_on_low_disk_space").value());

    transfersGrid->AddGrowableCol(1, 1);
    transfersGrid->Add(new wxStaticText(transfersSizer->GetStaticBox(), wxID_ANY, i18n("save_path")), 0, wxALIGN_CENTER_VERTICAL);
    transfersGrid->Add(m_savePathCtrl, 1, wxEXPAND);

    transfersSizer->Add(transfersGrid, 1, wxEXPAND | wxALL, 5);
    transfersSizer->Add(m_moveCompletedEnabled, 0, wxALL, 5);

    wxBoxSizer* bs1 = new wxBoxSizer(wxHORIZONTAL);
    bs1->AddSpacer(15);
    bs1->Add(m_moveCompletedPathCtrl, 1, wxEXPAND);

    wxBoxSizer* bs2 = new wxBoxSizer(wxHORIZONTAL);
    bs2->AddSpacer(15);
    bs2->Add(m_moveCompletedOnlyFromDefault);

    transfersSizer->Add(bs1, 0, wxEXPAND | wxALL, 5);
    transfersSizer->Add(bs2, 0, wxALL, 5);
    transfersSizer->Add(new wxStaticLine(transfersSizer->GetStaticBox(), wxID_ANY), 0, wxEXPAND | wxALL, 5);
    transfersSizer->Add(m_pauseLowDiskSpace, 0, wxALL, 5);

    wxStaticBoxSizer* limitsSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("limits"));

    /* Rate limits */
    wxFlexGridSizer* transferLimitsGrid = new wxFlexGridSizer(3, 10, 10);

    m_enableDownloadLimit = new wxCheckBox(limitsSizer->GetStaticBox(), wxID_ANY, i18n("dl_limit"));
    m_enableDownloadLimit->SetValue(m_cfg->Get<bool>("libtorrent.enable_download_rate_limit").value());

    m_downloadLimit = new wxTextCtrl(limitsSizer->GetStaticBox(), wxID_ANY);
    m_downloadLimit->Enable(m_cfg->Get<bool>("libtorrent.enable_download_rate_limit").value());
    m_downloadLimit->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    m_downloadLimit->SetValue(std::to_string(m_cfg->Get<int>("libtorrent.download_rate_limit").value()));

    m_enableUploadLimit = new wxCheckBox(limitsSizer->GetStaticBox(), wxID_ANY, i18n("ul_limit"));
    m_enableUploadLimit->SetValue(m_cfg->Get<bool>("libtorrent.enable_upload_rate_limit").value());

    m_uploadLimit = new wxTextCtrl(limitsSizer->GetStaticBox(), wxID_ANY);
    m_uploadLimit->Enable(m_cfg->Get<bool>("libtorrent.enable_upload_rate_limit").value());
    m_uploadLimit->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    m_uploadLimit->SetValue(std::to_string(m_cfg->Get<int>("libtorrent.upload_rate_limit").value()));

    m_enableDownloadLimit->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) { m_downloadLimit->Enable(m_enableDownloadLimit->GetValue()); });
    m_enableUploadLimit->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) { m_uploadLimit->Enable(m_enableUploadLimit->GetValue()); });

    transferLimitsGrid->AddGrowableCol(1, 1);
    transferLimitsGrid->Add(m_enableDownloadLimit, 0, wxALIGN_CENTER_VERTICAL);
    transferLimitsGrid->Add(m_downloadLimit, 0, wxALIGN_RIGHT);
    transferLimitsGrid->Add(new wxStaticText(limitsSizer->GetStaticBox(), wxID_ANY, "KB/s"), 0, wxALIGN_CENTER_VERTICAL);
    transferLimitsGrid->Add(m_enableUploadLimit, 0, wxALIGN_CENTER_VERTICAL);
    transferLimitsGrid->Add(m_uploadLimit, 0, wxALIGN_RIGHT);
    transferLimitsGrid->Add(new wxStaticText(limitsSizer->GetStaticBox(), wxID_ANY, "KB/s"), 0, wxALIGN_CENTER_VERTICAL);

    /* Active limits */
    wxFlexGridSizer* activeLimitsGrid = new wxFlexGridSizer(2, 10, 10);

    m_activeLimit = new wxTextCtrl(limitsSizer->GetStaticBox(), wxID_ANY);
    m_activeLimit->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    m_activeLimit->SetValue(std::to_string(m_cfg->Get<int>("libtorrent.active_limit").value()));

    m_activeDownloadsLimit = new wxTextCtrl(limitsSizer->GetStaticBox(), wxID_ANY);
    m_activeDownloadsLimit->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    m_activeDownloadsLimit->SetValue(std::to_string(m_cfg->Get<int>("libtorrent.active_downloads").value()));

    m_activeSeedsLimit = new wxTextCtrl(limitsSizer->GetStaticBox(), wxID_ANY);
    m_activeSeedsLimit->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    m_activeSeedsLimit->SetValue(std::to_string(m_cfg->Get<int>("libtorrent.active_seeds").value()));

    m_connectionsLimit = new wxTextCtrl(limitsSizer->GetStaticBox(), wxID_ANY);
    m_connectionsLimit->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    m_connectionsLimit->SetValue(std::to_string(m_cfg->Get<int>("libtorrent.connections_limit").value()));

    activeLimitsGrid->AddGrowableCol(0, 1);
    activeLimitsGrid->Add(new wxStaticText(limitsSizer->GetStaticBox(), wxID_ANY, i18n("total_active")));
    activeLimitsGrid->Add(m_activeLimit, 0, wxALIGN_RIGHT);
    activeLimitsGrid->Add(new wxStaticText(limitsSizer->GetStaticBox(), wxID_ANY, i18n("active_downloads")));
    activeLimitsGrid->Add(m_activeDownloadsLimit, 0, wxALIGN_RIGHT);
    activeLimitsGrid->Add(new wxStaticText(limitsSizer->GetStaticBox(), wxID_ANY, i18n("active_seeds")));
    activeLimitsGrid->Add(m_activeSeedsLimit, 0, wxALIGN_RIGHT);
    activeLimitsGrid->Add(new wxStaticText(limitsSizer->GetStaticBox(), wxID_ANY, i18n("connections")));
    activeLimitsGrid->Add(m_connectionsLimit, 0, wxALIGN_RIGHT);

    limitsSizer->Add(transferLimitsGrid, 0, wxEXPAND | wxALL, 5);
    limitsSizer->Add(new wxStaticLine(limitsSizer->GetStaticBox(), wxID_ANY), 0, wxEXPAND | wxALL, 5);
    limitsSizer->Add(activeLimitsGrid, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(transfersSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(limitsSizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);

    m_moveCompletedEnabled->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&)
    {
        m_moveCompletedPathCtrl->Enable(m_moveCompletedEnabled->IsChecked());
        m_moveCompletedOnlyFromDefault->Enable(m_moveCompletedEnabled->IsChecked());
    });

    m_moveCompletedEnabled->SetValue(cfg->Get<bool>("move_completed_downloads").value());
    m_moveCompletedPathCtrl->SetPath(Utils::toStdWString(cfg->Get<std::string>("move_completed_downloads_path").value_or("")));
    m_moveCompletedOnlyFromDefault->SetValue(cfg->Get<bool>("move_completed_downloads_from_default_only").value());

    m_moveCompletedPathCtrl->Enable(m_moveCompletedEnabled->IsChecked());
    m_moveCompletedOnlyFromDefault->Enable(m_moveCompletedEnabled->IsChecked());
}

void PreferencesDownloadsPage::Save()
{
    long dlLimit = 0;
    m_downloadLimit->GetValue().ToLong(&dlLimit);

    long ulLimit = 0;
    m_uploadLimit->GetValue().ToLong(&ulLimit);

    m_cfg->Set("default_save_path", Utils::toStdString(m_savePathCtrl->GetPath().ToStdWstring()));
    m_cfg->Set("libtorrent.enable_download_rate_limit", m_enableDownloadLimit->GetValue());
    m_cfg->Set("libtorrent.download_rate_limit", static_cast<int>(dlLimit));
    m_cfg->Set("libtorrent.enable_upload_rate_limit", m_enableUploadLimit->GetValue());
    m_cfg->Set("libtorrent.upload_rate_limit", static_cast<int>(ulLimit));

    m_cfg->Set("pause_on_low_disk_space", m_pauseLowDiskSpace->IsChecked());

    // Move
    m_cfg->Set("move_completed_downloads", m_moveCompletedEnabled->IsChecked());
    m_cfg->Set("move_completed_downloads_from_default_only", m_moveCompletedOnlyFromDefault->IsChecked());
    m_cfg->Set("move_completed_downloads_path", Utils::toStdString(m_moveCompletedPathCtrl->GetPath().ToStdWstring()));

    // Active limits
    long activeLimit = 0;
    m_activeLimit->GetValue().ToLong(&activeLimit);

    long activeDownloads = 0;
    m_activeDownloadsLimit->GetValue().ToLong(&activeDownloads);

    long activeSeeds = 0;
    m_activeSeedsLimit->GetValue().ToLong(&activeSeeds);

    long connectionsLimit = 0;
    m_connectionsLimit->GetValue().ToLong(&connectionsLimit);

    m_cfg->Set("libtorrent.active_limit", static_cast<int>(activeLimit));
    m_cfg->Set("libtorrent.active_downloads", static_cast<int>(activeDownloads));
    m_cfg->Set("libtorrent.active_seeds", static_cast<int>(activeSeeds));
    m_cfg->Set("libtorrent.connections_limit", static_cast<int>(connectionsLimit));
}

bool PreferencesDownloadsPage::IsValid()
{
    return true;
}
