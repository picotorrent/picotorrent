#include "advancedpage.hpp"

#include "config.hpp"
#include "scaler.hpp"
#include "string.hpp"
#include "translator.hpp"

#include <wx/propgrid/propgrid.h>

using pt::AdvancedPage;

AdvancedPage::AdvancedPage(wxWindow* parent, std::shared_ptr<pt::Configuration> config, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, wxID_ANY),
    m_parent(parent),
    m_cfg(config),
    m_translator(tr)
{
    m_pg = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_SPLITTER_AUTO_CENTER);
    m_pg->Append(new wxBoolProperty(i18n(tr, "encrypt_config_file"), "EncryptConfigFile", config->EncryptConfigurationFile()));
    m_pg->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_pg, 1, wxEXPAND | wxLEFT | wxRIGHT, SX(5));

    this->SetSizerAndFit(sizer);
}

void AdvancedPage::ApplyConfiguration()
{
    m_cfg->EncryptConfigurationFile(m_pg->GetProperty("EncryptConfigFile")->GetValue());
}

bool AdvancedPage::ValidateConfiguration(wxString& error)
{
    return true;
}
