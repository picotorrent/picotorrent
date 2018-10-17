#include "connectionpage.hpp"

#include "config.hpp"
#include "scaler.hpp"
#include "string.hpp"
#include "translator.hpp"

using pt::ConnectionPage;

ConnectionPage::ConnectionPage(wxWindow* parent, std::shared_ptr<pt::Configuration> config, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, wxID_ANY),
    m_parent(parent),
    m_cfg(config),
    m_translator(tr)
{
    wxStaticBoxSizer* listenSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "listen_interface"));
    wxFlexGridSizer* listenGrid = new wxFlexGridSizer(1, SY(10), SX(10));

    wxString val;

    for (auto ifs : m_cfg->ListenInterfaces())
    {
        val << ifs.first << ":" << ifs.second << "\n";
    }

    m_listenInterfaces = new wxTextCtrl(listenSizer->GetStaticBox(), wxID_ANY, val, wxDefaultPosition, wxSize(SX(100), SX(60)), wxTE_MULTILINE);

    listenGrid->AddGrowableCol(0, 1);
    listenGrid->Add(m_listenInterfaces, 1, wxEXPAND);
    listenSizer->Add(listenGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* encryptionSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "encryption"));
    wxFlexGridSizer* encryptionGrid = new wxFlexGridSizer(1, SY(10), SX(10));

    m_incomingEncryption = new wxCheckBox(encryptionSizer->GetStaticBox(), wxID_ANY, i18n(tr, "require_encryption_incoming"));
    m_incomingEncryption->SetValue(m_cfg->Session()->RequireIncomingEncryption());

    m_outgoingEncryption = new wxCheckBox(encryptionSizer->GetStaticBox(), wxID_ANY, i18n(tr, "require_encryption_outgoing"));
    m_outgoingEncryption->SetValue(m_cfg->Session()->RequireOutgoingEncryption());

    encryptionGrid->AddGrowableCol(0, 1);
    encryptionGrid->Add(m_incomingEncryption, 1, wxEXPAND);
    encryptionGrid->Add(m_outgoingEncryption, 1, wxEXPAND);
    encryptionSizer->Add(encryptionGrid, 1, wxEXPAND | wxALL, SX(5));

    wxStaticBoxSizer* privacySizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "privacy"));
    wxFlexGridSizer* privacyGrid = new wxFlexGridSizer(3, SY(10), SX(10));

    m_enableDht = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n(tr, "enable_dht"));
    m_enableDht->SetValue(m_cfg->Session()->EnableDht());

    m_enableLsd = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n(tr, "enable_lsd"));
    m_enableLsd->SetValue(m_cfg->Session()->EnableLsd());

    m_enablePex = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n(tr, "enable_pex"));
    m_enablePex->SetValue(m_cfg->Session()->EnablePex());

    privacyGrid->AddGrowableCol(0, 1);
    privacyGrid->AddGrowableCol(1, 1);
    privacyGrid->AddGrowableCol(2, 1);
    privacyGrid->Add(m_enableDht, 1, wxEXPAND);
    privacyGrid->Add(m_enableLsd, 1, wxEXPAND);
    privacyGrid->Add(m_enablePex, 1, wxEXPAND);
    privacySizer->Add(privacyGrid, 1, wxEXPAND | wxALL, SX(5));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(listenSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, SX(5));
    sizer->AddSpacer(SY(10));
    sizer->Add(encryptionSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, SX(5));
    sizer->AddSpacer(SY(10));
    sizer->Add(privacySizer, 0, wxEXPAND | wxLEFT | wxRIGHT, SX(5));
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);
}

void ConnectionPage::ApplyConfiguration()
{
    std::vector<std::pair<std::string, int>> ifs;
    auto pairs = String::Split(m_listenInterfaces->GetValue().ToStdString(), "\n");

    for (auto pair : pairs)
    {
        if (pair.empty()) { continue; }

        size_t colonIndex = pair.find_last_of(":");
        std::string addr = pair.substr(0, colonIndex);
        std::string port = pair.substr(colonIndex + 1);

        ifs.push_back({ addr, std::stoi(port) });
    }

    m_cfg->ListenInterfaces(ifs);
    m_cfg->Session()->RequireIncomingEncryption(m_incomingEncryption->GetValue());
    m_cfg->Session()->RequireOutgoingEncryption(m_outgoingEncryption->GetValue());

    m_cfg->Session()->EnableDht(m_enableDht->GetValue());
    m_cfg->Session()->EnableLsd(m_enableLsd->GetValue());
    m_cfg->Session()->EnablePex(m_enablePex->GetValue());
}

bool ConnectionPage::ValidateConfiguration(wxString& error)
{
    if (m_enablePex->GetValue() != m_cfg->Session()->EnablePex())
    {
        wxMessageBox(
            i18n(m_translator, "changing_pex_settings_requires_restart"),
            i18n(m_translator, "restart_required"),
            wxOK | wxCENTRE,
            m_parent);
    }

    return true;
}
