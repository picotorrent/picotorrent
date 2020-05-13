#include "listeninterfacedialog.hpp"

#include <sstream>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#include <wx/hyperlink.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>

#include "../clientdata.hpp"
#include "../translator.hpp"
#include "../../core/utils.hpp"

struct NetworkAdapter
{
    std::string name;
    std::string address;
};

using pt::UI::Dialogs::ListenInterfaceDialog;

ListenInterfaceDialog::ListenInterfaceDialog(wxWindow* parent, wxWindowID id, std::string address, int port)
    : wxDialog(parent, id, wxEmptyString)
{
    auto grid = new wxFlexGridSizer(2, FromDIP(5), FromDIP(5));
    grid->AddGrowableCol(1, 1);

    m_adapters = new wxChoice(this, wxID_ANY);
    m_port = new wxTextCtrl(this, wxID_ANY);

    grid->Add(new wxStaticText(this, wxID_ANY, i18n("network_adapter")));
    grid->Add(m_adapters, 1, wxEXPAND | wxALL);
    grid->Add(new wxStaticText(this, wxID_ANY, i18n("port")));
    grid->Add(m_port);

    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();
    buttonSizer->Add(new wxButton(this, wxID_OK, i18n("ok")));
    buttonSizer->Add(new wxButton(this, wxID_CANCEL, i18n("cancel")));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(grid, 1, wxEXPAND | wxALL, FromDIP(10));
    sizer->Add(buttonSizer, 0, wxEXPAND | wxBOTTOM | wxRIGHT, FromDIP(10));

    this->SetSizerAndFit(sizer);
    this->SetTitle(i18n("listen_interface"));

    wxSize size{ FromDIP(380), this->GetSize().GetHeight() };
    this->SetSize(size);

    this->LoadAdapters();

    if (address.size() > 0)
    {
        for (unsigned int i = 0; i < m_adapters->GetCount(); i++)
        {
            auto na = static_cast<ClientData<NetworkAdapter>*>(m_adapters->GetClientObject(i));

            if (na->GetValue().address == address)
            {
                m_adapters->SetSelection(i);
                break;
            }
        }
    }

    if (port > 0)
    {
        m_port->SetValue(std::to_string(port));
    }
}

ListenInterfaceDialog::~ListenInterfaceDialog()
{
}

std::string ListenInterfaceDialog::GetAddress()
{
    auto na = static_cast<ClientData<NetworkAdapter>*>(m_adapters->GetClientObject(m_adapters->GetSelection()));
    return na->GetValue().address;
}

int ListenInterfaceDialog::GetPort()
{
    return std::atoi(m_port->GetValue());
}

void ListenInterfaceDialog::LoadAdapters()
{
    NetworkAdapter any4;
    any4.address = "0.0.0.0";
    any4.name = "0.0.0.0";

    NetworkAdapter any6;
    any6.address = "[::]";
    any6.name = "[::]";

    m_adapters->Insert(
        any4.name,
        m_adapters->GetCount(),
        new ClientData<NetworkAdapter>(any4));

    m_adapters->Insert(
        any6.name,
        m_adapters->GetCount(),
        new ClientData<NetworkAdapter>(any6));

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
        {
            if (pCurrAddresses->IfType != IF_TYPE_SOFTWARE_LOOPBACK)
            {
                IP_ADAPTER_UNICAST_ADDRESS* pu = pCurrAddresses->FirstUnicastAddress;

                while (pu)
                {
                    switch (pu->Address.lpSockaddr->sa_family)
                    {
                    case AF_INET:
                    {
                        char buf[INET_ADDRSTRLEN] = {};
                        sockaddr_in* si = reinterpret_cast<sockaddr_in*>(pu->Address.lpSockaddr);
                        inet_ntop(AF_INET, &(si->sin_addr), buf, sizeof(buf));

                        std::stringstream ss;
                        ss << buf << " (" << Utils::toStdString(pCurrAddresses->FriendlyName) << ")";

                        NetworkAdapter ipv4;
                        ipv4.address = buf;
                        ipv4.name = ss.str();

                        m_adapters->Insert(
                            ipv4.name,
                            m_adapters->GetCount(),
                            new ClientData<NetworkAdapter>(ipv4));

                        break;
                    }
                    case AF_INET6:
                    {
                        char buf[INET6_ADDRSTRLEN] = {};
                        sockaddr_in6* si = reinterpret_cast<sockaddr_in6*>(pu->Address.lpSockaddr);
                        inet_ntop(AF_INET6, &(si->sin6_addr), buf, sizeof(buf));

                        std::stringstream ss;
                        ss << "[" << buf << "]" << " (" << Utils::toStdString(pCurrAddresses->FriendlyName) << ")";

                        NetworkAdapter ipv6;
                        ipv6.address = "[" + std::string(buf) + "]";
                        ipv6.name = ss.str();

                        m_adapters->Insert(
                            ipv6.name,
                            m_adapters->GetCount(),
                            new ClientData<NetworkAdapter>(ipv6));

                        break;
                    }
                    }

                    pu = pu->Next;
                }
            }
        }
    }

    delete[] pAddresses;
}
