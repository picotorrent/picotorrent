#include "downloadspage.hpp"

#include "translator.hpp"

#include <wx/filepicker.h>

using pt::DownloadsPage;

DownloadsPage::DownloadsPage(wxWindow* parent, std::shared_ptr<pt::Translator> tran)
    : wxPanel(parent, wxID_ANY)
{
    wxStaticBoxSizer* transfersSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tran, "transfers"));
    wxFlexGridSizer* transfersGrid = new wxFlexGridSizer(2, 10, 10);
    transfersGrid->AddGrowableCol(1, 1);
    transfersGrid->Add(new wxStaticText(transfersSizer->GetStaticBox(), wxID_ANY, i18n(tran, "save_path")), 0, wxALIGN_CENTER_VERTICAL);
    transfersGrid->Add(new wxDirPickerCtrl(transfersSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL), 1, wxEXPAND);
    transfersSizer->Add(transfersGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* limitsSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tran, "limits"));
    wxFlexGridSizer* limitsGrid = new wxFlexGridSizer(3, 10, 10);
    limitsGrid->AddGrowableCol(1, 1);
    limitsGrid->Add(new wxCheckBox(limitsSizer->GetStaticBox(), wxID_ANY, i18n(tran, "dl_limit")), 0, wxALIGN_CENTER_VERTICAL);
    limitsGrid->Add(new wxTextCtrl(limitsSizer->GetStaticBox(), wxID_ANY), 0, wxALIGN_RIGHT);
    limitsGrid->Add(new wxStaticText(limitsSizer->GetStaticBox(), wxID_ANY, "KB/s"), 0, wxALIGN_CENTER_VERTICAL);
    limitsGrid->Add(new wxCheckBox(limitsSizer->GetStaticBox(), wxID_ANY, i18n(tran, "ul_limit")), 0, wxALIGN_CENTER_VERTICAL);
    limitsGrid->Add(new wxTextCtrl(limitsSizer->GetStaticBox(), wxID_ANY), 0, wxALIGN_RIGHT);
    limitsGrid->Add(new wxStaticText(limitsSizer->GetStaticBox(), wxID_ANY, "KB/s"), 0, wxALIGN_CENTER_VERTICAL);
    limitsSizer->Add(limitsGrid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(transfersSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(limitsSizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);
}
