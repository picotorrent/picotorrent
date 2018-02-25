#include "proxypage.hpp"

#include "clientdata.hpp"
#include "config.hpp"
#include "translator.hpp"

using pt::ProxyPage;

ProxyPage::ProxyPage(wxWindow* parent, std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, wxID_ANY),
    m_cfg(cfg)
{
    wxStaticBoxSizer* proxySizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "proxy"));
    wxFlexGridSizer* proxyGrid = new wxFlexGridSizer(2, 10, 10);

    m_type = new wxChoice(proxySizer->GetStaticBox(), wxID_ANY);
    m_host = new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY);
    m_port = new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY);
    m_port->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    m_username = new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY);
    m_password = new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    m_forceProxy = new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "force_proxy"));
    m_proxyHostnames = new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "proxy_hostnames"));
    m_proxyPeers = new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "proxy_peer_connections"));
    m_proxyTrackers = new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "proxy_tracker_connections"));

    proxyGrid->AddGrowableCol(1, 1);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "type")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_type, 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "host")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_host, 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "port")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_port, 1);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "username")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_username, 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "password")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_password, 1, wxEXPAND);

    wxFlexGridSizer* proxySettingsGrid = new wxFlexGridSizer(2, 10, 10);
    proxyGrid->Add(m_forceProxy, 1, wxEXPAND);
    proxyGrid->Add(m_proxyHostnames, 1, wxEXPAND);
    proxyGrid->Add(m_proxyPeers, 1, wxEXPAND);
    proxyGrid->Add(m_proxyTrackers, 1, wxEXPAND);

    proxySizer->Add(proxyGrid, 1, wxEXPAND | wxALL, 5);
    proxySizer->Add(proxySettingsGrid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(proxySizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    m_type->Bind(wxEVT_CHOICE, [this](wxCommandEvent&) { UpdateUI(); });

    m_type->Append(i18n(tr, "none"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::None));
    m_type->Append(i18n(tr, "http"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::HTTP));
    m_type->Append(i18n(tr, "http_with_credentials"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::HTTP_Password));
    m_type->Append(i18n(tr, "i2p"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::I2P));
    m_type->Append(i18n(tr, "socks4"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::SOCKS4));
    m_type->Append(i18n(tr, "socks5"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::SOCKS5));
    m_type->Append(i18n(tr, "socks5_with_credentials"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::SOCKS5_Password));

    for (unsigned int i = 0; i < m_type->GetCount(); i++)
    {
        auto clientData = reinterpret_cast<ClientData<Configuration::ConnectionProxyType>*>(m_type->GetClientObject(i));

        if (clientData->GetValue() == m_cfg->ProxyType())
        {
            m_type->Select(i);
            break;
        }
    }

    m_host->SetValue(m_cfg->ProxyHost());

    if (m_cfg->ProxyPort() > 0)
    {
        m_port->SetValue(wxString::Format("%i", m_cfg->ProxyPort()));
    }

    m_username->SetValue(m_cfg->ProxyUsername());
    m_password->SetValue(m_cfg->ProxyPassword());
    m_forceProxy->SetValue(m_cfg->ProxyForce());
    m_proxyHostnames->SetValue(m_cfg->ProxyHostnames());
    m_proxyPeers->SetValue(m_cfg->ProxyPeers());
    m_proxyTrackers->SetValue(m_cfg->ProxyTrackers());

    this->SetSizerAndFit(sizer);
    this->UpdateUI();
}

void ProxyPage::ApplyConfiguration()
{
    int typeIndex = m_type->GetSelection();
    auto typeData = reinterpret_cast<ClientData<Configuration::ConnectionProxyType>*>(m_type->GetClientObject(typeIndex));

    long proxyPort = 0;
    m_port->GetValue().ToLong(&proxyPort);

    m_cfg->ProxyType(typeData->GetValue());
    m_cfg->ProxyHost(m_host->GetValue().ToStdString());
    m_cfg->ProxyPort(static_cast<int>(proxyPort));
    m_cfg->ProxyUsername(m_username->GetValue().ToStdString());
    m_cfg->ProxyPassword(m_password->GetValue().ToStdString());
    m_cfg->ProxyForce(m_forceProxy->GetValue());
    m_cfg->ProxyHostnames(m_proxyHostnames->GetValue());
    m_cfg->ProxyPeers(m_proxyPeers->GetValue());
    m_cfg->ProxyTrackers(m_proxyTrackers->GetValue());
}

bool ProxyPage::ValidateConfiguration(wxString& error)
{
    return true;
}

void ProxyPage::UpdateUI()
{
    int typeIndex = m_type->GetSelection();
    auto data = reinterpret_cast<ClientData<Configuration::ConnectionProxyType>*>(m_type->GetClientObject(typeIndex));

    m_host->Enable(false);
    m_port->Enable(false);
    m_username->Enable(false);
    m_password->Enable(false);
    m_forceProxy->Enable(false);
    m_proxyHostnames->Enable(false);
    m_proxyPeers->Enable(false);
    m_proxyTrackers->Enable(false);

    switch (data->GetValue())
    {
    case Configuration::ConnectionProxyType::SOCKS4:
    case Configuration::ConnectionProxyType::SOCKS5:
    case Configuration::ConnectionProxyType::SOCKS5_Password:
    case Configuration::ConnectionProxyType::HTTP:
    case Configuration::ConnectionProxyType::HTTP_Password:
    case Configuration::ConnectionProxyType::I2P:
    {
        m_host->Enable(true);
        m_port->Enable(true);
        m_forceProxy->Enable(true);
        m_proxyHostnames->Enable(true);
        m_proxyPeers->Enable(true);
        m_proxyTrackers->Enable(true);
        break;
    }
    }

    switch (data->GetValue())
    {
    case Configuration::ConnectionProxyType::SOCKS5_Password:
    case Configuration::ConnectionProxyType::HTTP_Password:
    {
        m_username->Enable(true);
        m_password->Enable(true);
        break;
    }
    }
}
