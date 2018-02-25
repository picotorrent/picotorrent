#include "connectionpage.hpp"

#include "config.hpp"
#include "string.hpp"
#include "translator.hpp"

using pt::ConnectionPage;

ConnectionPage::ConnectionPage(wxWindow* parent, std::shared_ptr<pt::Configuration> config, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, wxID_ANY),
    m_cfg(config)
{
    wxStaticBoxSizer* listenSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "listen_interface"));
    wxFlexGridSizer* listenGrid = new wxFlexGridSizer(1, 10, 10);

    wxString val;

    for (auto ifs : m_cfg->ListenInterfaces())
    {
        val << ifs.first << ":" << ifs.second << "\n";
    }

    m_listenInterfaces = new wxTextCtrl(listenSizer->GetStaticBox(), wxID_ANY, val, wxDefaultPosition, wxSize(100, 60), wxTE_MULTILINE);

    listenGrid->AddGrowableCol(0, 1);
    listenGrid->Add(m_listenInterfaces, 1, wxEXPAND);
    listenSizer->Add(listenGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* privacySizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "encryption"));
    wxFlexGridSizer* privacyGrid = new wxFlexGridSizer(1, 10, 10);

    m_incomingEncryption = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n(tr, "require_encryption_incoming"));
    m_incomingEncryption->SetValue(m_cfg->Session()->RequireIncomingEncryption());

    m_outgoingEncryption = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n(tr, "require_encryption_outgoing"));
    m_outgoingEncryption->SetValue(m_cfg->Session()->RequireOutgoingEncryption());

    privacyGrid->AddGrowableCol(0, 1);
    privacyGrid->Add(m_incomingEncryption, 1, wxEXPAND);
    privacyGrid->Add(m_outgoingEncryption, 1, wxEXPAND);
    privacySizer->Add(privacyGrid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(listenSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(privacySizer, 0, wxEXPAND);
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
}

bool ConnectionPage::ValidateConfiguration(wxString& error)
{
    return true;
}
