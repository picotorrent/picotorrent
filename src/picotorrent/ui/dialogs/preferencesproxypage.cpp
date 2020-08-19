#include "preferencesproxypage.hpp"

#include "../clientdata.hpp"
#include "../../core/configuration.hpp"
#include "../translator.hpp"

using pt::Core::Configuration;
using pt::UI::Dialogs::PreferencesProxyPage;

PreferencesProxyPage::PreferencesProxyPage(wxWindow* parent, std::shared_ptr<Configuration> cfg)
    : wxPanel(parent, wxID_ANY),
    m_cfg(cfg)
{
    wxStaticBoxSizer* proxySizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("proxy"));
    wxFlexGridSizer* proxyGrid = new wxFlexGridSizer(2, 10, 10);

    m_type = new wxChoice(proxySizer->GetStaticBox(), wxID_ANY);
    m_host = new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY);
    m_port = new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY);
    m_port->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    m_username = new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY);
    m_password = new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    m_proxyHostnames = new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n("proxy_hostnames"));
    m_proxyPeers = new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n("proxy_peer_connections"));
    m_proxyTrackers = new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n("proxy_tracker_connections"));

    proxyGrid->AddGrowableCol(1, 1);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n("type")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_type, 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n("host")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_host, 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n("port")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_port, 1);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n("username")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_username, 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n("password")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(m_password, 1, wxEXPAND);

    wxFlexGridSizer* proxySettingsGrid = new wxFlexGridSizer(2, 10, 10);
    proxyGrid->Add(m_proxyHostnames, 1, wxEXPAND);
    proxyGrid->Add(m_proxyPeers, 1, wxEXPAND);
    proxyGrid->Add(m_proxyTrackers, 1, wxEXPAND);

    proxySizer->Add(proxyGrid, 1, wxEXPAND | wxALL, 5);
    proxySizer->Add(proxySettingsGrid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(proxySizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    m_type->Bind(wxEVT_CHOICE, [this](wxCommandEvent&) { UpdateUI(); });

    m_type->Append(i18n("none"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::None));
    m_type->Append(i18n("http"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::HTTP));
    m_type->Append(i18n("http_with_credentials"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::HTTP_Password));
    m_type->Append(i18n("socks4"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::SOCKS4));
    m_type->Append(i18n("socks5"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::SOCKS5));
    m_type->Append(i18n("socks5_with_credentials"), new ClientData<Configuration::ConnectionProxyType>(Configuration::ConnectionProxyType::SOCKS5_Password));

    for (unsigned int i = 0; i < m_type->GetCount(); i++)
    {
        auto clientData = reinterpret_cast<ClientData<Configuration::ConnectionProxyType>*>(m_type->GetClientObject(i));

        if (clientData->GetValue() == static_cast<Configuration::ConnectionProxyType>(m_cfg->GetInt("proxy_type")))
        {
            m_type->Select(i);
            break;
        }
    }

    m_host->SetValue(m_cfg->GetString("proxy_host"));

    if (m_cfg->GetInt("proxy_port") > 0)
    {
        m_port->SetValue(std::to_string(m_cfg->GetInt("proxy_port")));
    }

    m_username->SetValue(m_cfg->GetString("proxy_username"));
    m_password->SetValue(m_cfg->GetString("proxy_password"));
    m_proxyHostnames->SetValue(m_cfg->GetBool("proxy_hostnames"));
    m_proxyPeers->SetValue(m_cfg->GetBool("proxy_peers"));
    m_proxyTrackers->SetValue(m_cfg->GetBool("proxy_trackers"));

    this->SetSizerAndFit(sizer);
    this->UpdateUI();
}

void PreferencesProxyPage::Save()
{
    int typeIndex = m_type->GetSelection();
    auto typeData = reinterpret_cast<ClientData<Configuration::ConnectionProxyType>*>(m_type->GetClientObject(typeIndex));

    long proxyPort = 0;
    m_port->GetValue().ToLong(&proxyPort);

    m_cfg->SetInt("proxy_type", typeData->GetValue());
    m_cfg->SetString("proxy_host", m_host->GetValue().ToStdString());
    m_cfg->SetInt("proxy_port", static_cast<int>(proxyPort));
    m_cfg->SetString("proxy_username", m_username->GetValue().ToStdString());
    m_cfg->SetString("proxy_password", m_password->GetValue().ToStdString());
    m_cfg->SetBool("proxy_hostnames", m_proxyHostnames->GetValue());
    m_cfg->SetBool("proxy_peers", m_proxyPeers->GetValue());
    m_cfg->SetBool("proxy_trackers", m_proxyTrackers->GetValue());
}

bool PreferencesProxyPage::IsValid()
{
    return true;
}

void PreferencesProxyPage::UpdateUI()
{
    int typeIndex = m_type->GetSelection();
    auto data = reinterpret_cast<ClientData<Configuration::ConnectionProxyType>*>(m_type->GetClientObject(typeIndex));

    m_host->Enable(false);
    m_port->Enable(false);
    m_username->Enable(false);
    m_password->Enable(false);
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
    {
        m_host->Enable(true);
        m_port->Enable(true);
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
