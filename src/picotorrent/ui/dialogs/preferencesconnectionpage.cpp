#include "preferencesconnectionpage.hpp"

#include <sstream>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#include <wx/tokenzr.h>

#include "../clientdata.hpp"
#include "../../core/configuration.hpp"
#include "../../core/utils.hpp"
#include "../translator.hpp"

struct NetworkAdapter
{
    wxString description;
    wxString ipv4;
    wxString ipv6;
};

using pt::UI::Dialogs::PreferencesConnectionPage;

PreferencesConnectionPage::PreferencesConnectionPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg)
    : wxPanel(parent, wxID_ANY),
    m_parent(parent),
    m_cfg(cfg)
{
    wxStaticBoxSizer* listenSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("listen_interface"));
    wxFlexGridSizer* listenGrid = new wxFlexGridSizer(2, 10, 10);

    m_listenInterfaces = new wxChoice(listenSizer->GetStaticBox(), wxID_ANY);
    m_listenPort = new wxTextCtrl(listenSizer->GetStaticBox(), wxID_ANY, std::to_string(m_cfg->GetInt("listen_port")));

    listenGrid->AddGrowableCol(1, 1);
    listenGrid->Add(new wxStaticText(listenSizer->GetStaticBox(), wxID_ANY, i18n("network_adapter")));
    listenGrid->Add(m_listenInterfaces, 1, wxEXPAND);
    listenGrid->Add(new wxStaticText(listenSizer->GetStaticBox(), wxID_ANY, i18n("port")));
    listenGrid->Add(m_listenPort, 0, wxALIGN_RIGHT);
    listenSizer->Add(listenGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* encryptionSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("encryption"));
    wxFlexGridSizer* encryptionGrid = new wxFlexGridSizer(1, 10, 10);

    m_incomingEncryption = new wxCheckBox(encryptionSizer->GetStaticBox(), wxID_ANY, i18n("require_encryption_incoming"));
    m_incomingEncryption->SetValue(m_cfg->GetBool("require_incoming_encryption"));

    m_outgoingEncryption = new wxCheckBox(encryptionSizer->GetStaticBox(), wxID_ANY, i18n("require_encryption_outgoing"));
    m_outgoingEncryption->SetValue(m_cfg->GetBool("require_outgoing_encryption"));

    encryptionGrid->AddGrowableCol(0, 1);
    encryptionGrid->Add(m_incomingEncryption, 1, wxEXPAND);
    encryptionGrid->Add(m_outgoingEncryption, 1, wxEXPAND);
    encryptionSizer->Add(encryptionGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* privacySizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("privacy"));
    wxFlexGridSizer* privacyGrid = new wxFlexGridSizer(3, 10, 10);

    m_enableDht = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_dht"));
    m_enableDht->SetValue(m_cfg->GetBool("enable_dht"));

    m_enableLsd = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_lsd"));
    m_enableLsd->SetValue(m_cfg->GetBool("enable_lsd"));

    m_enablePex = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_pex"));
    m_enablePex->SetValue(m_cfg->GetBool("enable_pex"));

    privacyGrid->AddGrowableCol(0, 1);
    privacyGrid->AddGrowableCol(1, 1);
    privacyGrid->AddGrowableCol(2, 1);
    privacyGrid->Add(m_enableDht, 1, wxEXPAND);
    privacyGrid->Add(m_enableLsd, 1, wxEXPAND);
    privacyGrid->Add(m_enablePex, 1, wxEXPAND);
    privacySizer->Add(privacyGrid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(listenSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(encryptionSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(privacySizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);

    SetupNetworkAdapters();

    wxString iface = cfg->GetString("listen_interface");

    if (iface == "{any}")
    {
        m_listenInterfaces->SetSelection(0);
    }

    for (unsigned int i = 0; i < m_listenInterfaces->GetCount(); i++)
    {
        auto clientData = static_cast<ClientData<NetworkAdapter*>*>(m_listenInterfaces->GetClientObject(i));
        auto adapter = clientData->GetValue();

        if (adapter->ipv4 == iface || adapter->ipv6 == iface)
        {
            m_listenInterfaces->SetSelection(i);
        }
        else if (iface.Contains("|"))
        {
            auto pos = iface.find_first_of("|");
            auto first = iface.substr(0, pos);
            auto second = iface.substr(pos + 1);

            if (adapter->ipv4 == first || adapter->ipv6 == second)
            {
                m_listenInterfaces->SetSelection(i);
            }
        }
    }
}

void PreferencesConnectionPage::Save()
{
    auto clientData = static_cast<ClientData<NetworkAdapter*>*>(
        m_listenInterfaces->GetClientObject(m_listenInterfaces->GetSelection()));
    auto adapter = clientData->GetValue();

    std::stringstream ifaces;
    ifaces << adapter->ipv4;

    if (adapter->ipv6.size() > 0)
    {
        ifaces << "|" << adapter->ipv6;
    }

    long port;
    m_listenPort->GetValue().ToLong(&port);

    m_cfg->SetString("listen_interface", ifaces.str());
    m_cfg->SetInt("listen_port", static_cast<int>(port));

    m_cfg->SetBool("require_incoming_encryption", m_incomingEncryption->GetValue());
    m_cfg->SetBool("require_outgoing_encryption", m_outgoingEncryption->GetValue());

    m_cfg->SetBool("enable_dht", m_enableDht->GetValue());
    m_cfg->SetBool("enable_lsd", m_enableLsd->GetValue());
    m_cfg->SetBool("enable_pex", m_enablePex->GetValue());
}

bool PreferencesConnectionPage::IsValid()
{
    if (m_enablePex->GetValue() != m_cfg->GetBool("enable_pex"))
    {
        wxMessageBox(
            i18n("changing_pex_settings_requires_restart"),
            i18n("restart_required"),
            wxOK | wxCENTRE,
            m_parent);
    }

    return true;
}

void PreferencesConnectionPage::SetupNetworkAdapters()
{
    // Add the <any> adapter
    auto any = new NetworkAdapter();
    any->description = "<any>";
    any->ipv4 = "0.0.0.0";
    any->ipv6 = "::";

    m_listenInterfaces->Insert(
        any->description,
        m_listenInterfaces->GetCount(),
        new ClientData<NetworkAdapter*>(any));

    ULONG bufferSize = 15000;
    IP_ADAPTER_ADDRESSES* pAddresses = new IP_ADAPTER_ADDRESSES[bufferSize];

    ULONG ret = GetAdaptersAddresses(
        AF_UNSPEC,
        GAA_FLAG_INCLUDE_PREFIX,
        NULL,
        pAddresses,
        &bufferSize);

    if (ret == ERROR_SUCCESS)
    {
        for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != nullptr; pCurrAddresses = pCurrAddresses->Next)

            if (pCurrAddresses->IfType != IF_TYPE_SOFTWARE_LOOPBACK)
            {
                IP_ADAPTER_UNICAST_ADDRESS* pu = pCurrAddresses->FirstUnicastAddress;

                auto adapter = new NetworkAdapter();

                while (pu)
                {
                    switch (pu->Address.lpSockaddr->sa_family)
                    {
                    case AF_INET:
                    {
                        char buf[INET_ADDRSTRLEN] = {};
                        sockaddr_in* si = reinterpret_cast<sockaddr_in*>(pu->Address.lpSockaddr);
                        inet_ntop(AF_INET, &(si->sin_addr), buf, sizeof(buf));
                        adapter->ipv4 = buf;
                        break;
                    }
                    case AF_INET6:
                    {
                        char buf[INET6_ADDRSTRLEN] = {};
                        sockaddr_in6* si = reinterpret_cast<sockaddr_in6*>(pu->Address.lpSockaddr);
                        inet_ntop(AF_INET6, &(si->sin6_addr), buf, sizeof(buf));
                        adapter->ipv6 = buf;
                        break;
                    }
                    }

                    pu = pu->Next;
                }

                std::stringstream adapterDescription;
                adapterDescription << Utils::toStdString(pCurrAddresses->FriendlyName).c_str();
                adapterDescription << " (" << adapter->ipv4.c_str() << ")";

                adapter->description = adapterDescription.str();

                m_listenInterfaces->Insert(
                    adapter->description,
                    m_listenInterfaces->GetCount(),
                    new ClientData<NetworkAdapter*>(adapter));
            }
    }

    delete[] pAddresses;
}
