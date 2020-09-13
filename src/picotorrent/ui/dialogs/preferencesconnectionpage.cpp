#include "preferencesconnectionpage.hpp"

#include <sstream>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#include <wx/listctrl.h>
#include <wx/tokenzr.h>

#include "../clientdata.hpp"
#include "../../core/configuration.hpp"
#include "../../core/utils.hpp"
#include "listeninterfacedialog.hpp"
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
    wxStaticBoxSizer* listenSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, i18n("listen_interface"));
 
    m_listenInterfaces = new wxListView(listenSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    m_listenInterfaces->AppendColumn(i18n("address"), wxLIST_FORMAT_LEFT, FromDIP(180));
    m_listenInterfaces->AppendColumn(i18n("port"), wxLIST_FORMAT_RIGHT);

    auto buttonSizer = new wxBoxSizer(wxVERTICAL);
    auto addInterface = new wxButton(listenSizer->GetStaticBox(), wxID_ANY, "+");
    auto editInterface = new wxButton(listenSizer->GetStaticBox(), wxID_ANY, i18n("edit"));
    auto removeInterface = new wxButton(listenSizer->GetStaticBox(), wxID_ANY, "-");
    buttonSizer->Add(addInterface);
    buttonSizer->Add(editInterface);
    buttonSizer->Add(removeInterface);

    listenSizer->Add(m_listenInterfaces, 1, wxEXPAND | wxALL, FromDIP(5));
    listenSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, FromDIP(5));

    wxStaticBoxSizer* encryptionSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("encryption"));
    wxFlexGridSizer* encryptionGrid = new wxFlexGridSizer(1, 10, 10);

    m_incomingEncryption = new wxCheckBox(encryptionSizer->GetStaticBox(), wxID_ANY, i18n("require_encryption_incoming"));
    m_incomingEncryption->SetValue(m_cfg->Get<bool>("libtorrent.require_incoming_encryption").value());

    m_outgoingEncryption = new wxCheckBox(encryptionSizer->GetStaticBox(), wxID_ANY, i18n("require_encryption_outgoing"));
    m_outgoingEncryption->SetValue(m_cfg->Get<bool>("libtorrent.require_outgoing_encryption").value());

    encryptionGrid->AddGrowableCol(0, 1);
    encryptionGrid->Add(m_incomingEncryption, 1, wxEXPAND);
    encryptionGrid->Add(m_outgoingEncryption, 1, wxEXPAND);
    encryptionSizer->Add(encryptionGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* privacySizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("privacy"));
    wxFlexGridSizer* privacyGrid = new wxFlexGridSizer(3, 10, 10);

    m_enableDht = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_dht"));
    m_enableDht->SetValue(m_cfg->Get<bool>("libtorrent.enable_dht").value());

    m_enableLsd = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_lsd"));
    m_enableLsd->SetValue(m_cfg->Get<bool>("libtorrent.enable_lsd").value());

    m_enablePex = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_pex"));
    m_enablePex->SetValue(m_cfg->Get<bool>("libtorrent.enable_pex").value());

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

    for (auto const& li : cfg->GetListenInterfaces())
    {
        int row = m_listenInterfaces->GetItemCount();
        m_listenInterfaces->InsertItem(row, li.address);
        m_listenInterfaces->SetItem(row, 1, std::to_string(li.port));
        m_listenInterfaces->SetItemPtrData(row, li.id);
    }

    addInterface->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            ListenInterfaceDialog dlg(this, wxID_ANY);

            if (dlg.ShowModal() == wxID_OK)
            {
                int row = m_listenInterfaces->GetItemCount();
                m_listenInterfaces->InsertItem(row, dlg.GetAddress());
                m_listenInterfaces->SetItem(row, 1, std::to_string(dlg.GetPort()));
                m_listenInterfaces->SetItemPtrData(row, -1);
            }
        });

    editInterface->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            long sel = m_listenInterfaces->GetFirstSelected();
            if (sel < 0) { return; }

            std::string address = m_listenInterfaces->GetItemText(sel);
            int port = std::atoi(m_listenInterfaces->GetItemText(sel, 1));

            ListenInterfaceDialog dlg(this, wxID_ANY, address, port);

            if (dlg.ShowModal() == wxID_OK)
            {
                int row = m_listenInterfaces->GetItemCount();
                m_listenInterfaces->SetItem(sel, 0, dlg.GetAddress());
                m_listenInterfaces->SetItem(sel, 1, std::to_string(dlg.GetPort()));
            }
        });

    removeInterface->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            long sel = m_listenInterfaces->GetFirstSelected();
            if (sel < 0) { return; }

            int id = m_listenInterfaces->GetItemData(sel);
            m_listenInterfaces->DeleteItem(sel);

            if (id > 0)
            {
                m_removedListenInterfaces.push_back(id);
            }
        });
}

PreferencesConnectionPage::~PreferencesConnectionPage()
{
}

void PreferencesConnectionPage::Save()
{
    for (int removed : m_removedListenInterfaces)
    {
        m_cfg->DeleteListenInterface(removed);
    }

    for (int i = 0; i < m_listenInterfaces->GetItemCount(); i++)
    {
        Core::Configuration::ListenInterface li;
        li.address = m_listenInterfaces->GetItemText(i, 0);
        li.id = m_listenInterfaces->GetItemData(i);
        li.port = std::atoi(m_listenInterfaces->GetItemText(i, 1));

        m_cfg->UpsertListenInterface(li);
    }

    m_cfg->Set("libtorrent.require_incoming_encryption", m_incomingEncryption->GetValue());
    m_cfg->Set("libtorrent.require_outgoing_encryption", m_outgoingEncryption->GetValue());

    m_cfg->Set("libtorrent.enable_dht", m_enableDht->GetValue());
    m_cfg->Set("libtorrent.enable_lsd", m_enableLsd->GetValue());
    m_cfg->Set("libtorrent.enable_pex", m_enablePex->GetValue());
}

bool PreferencesConnectionPage::IsValid()
{
    if (m_enablePex->GetValue() != m_cfg->Get<bool>("libtorrent.enable_pex").value())
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
    /*// Add the <any> adapter
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

    delete[] pAddresses;*/
}
