#include "ConnectionPage.hpp"

#include <sstream>

#include "../../Configuration.hpp"
#include "../../Translator.hpp"

using PropertySheets::Preferences::ConnectionPage;

ConnectionPage::ConnectionPage()
{
    m_title = TRW("connection");
    SetTitle(m_title.c_str());
}

BOOL ConnectionPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    SetDlgItemText(ID_LISTEN_INTERFACE_GROUP, TRW("listen_interface"));
    SetDlgItemText(ID_PROXY_GROUP, TRW("proxy"));
    SetDlgItemText(ID_TYPE_TEXT, TRW("type"));
    SetDlgItemText(ID_HOST_TEXT, TRW("host"));
    SetDlgItemText(ID_PROXY_PORT_TEXT, TRW("port"));
    SetDlgItemText(ID_PROXY_USERNAME_TEXT, TRW("username"));
    SetDlgItemText(ID_PROXY_PASSWORD_TEXT, TRW("password"));
    SetDlgItemText(ID_PROXY_FORCE, TRW("force_proxy"));
    SetDlgItemText(ID_PROXY_HOSTNAMES, TRW("proxy_hostnames"));
    SetDlgItemText(ID_PROXY_PEERS, TRW("proxy_peer_connections"));
    SetDlgItemText(ID_PROXY_TRACKERS, TRW("proxy_tracker_connections"));

    // Load controls
    m_listenInterfaces = GetDlgItem(ID_LISTEN_INTERFACES);
    m_proxyType = GetDlgItem(ID_PROXY_TYPE);
    m_proxyHost = GetDlgItem(ID_PROXY_HOST);
    m_proxyPort = GetDlgItem(ID_PROXY_PORT);
    m_proxyUsername = GetDlgItem(ID_PROXY_USERNAME);
    m_proxyPassword = GetDlgItem(ID_PROXY_PASSWORD);
    m_proxyForce = GetDlgItem(ID_PROXY_FORCE);
    m_proxyHostnames = GetDlgItem(ID_PROXY_HOSTNAMES);
    m_proxyPeers = GetDlgItem(ID_PROXY_PEERS);
    m_proxyTrackers = GetDlgItem(ID_PROXY_TRACKERS);

    Configuration& cfg = Configuration::GetInstance();

    // Set up listen interfaces
    std::stringstream ifaces;
    for (auto& iface : cfg.GetListenInterfaces())
    {
        ifaces << iface.first << ":" << iface.second << "\r\n";
    }
    m_listenInterfaces.SetWindowText(ToWideString(ifaces.str()).c_str());

    // Set proxy settings
    m_proxyType.SetItemData(m_proxyType.AddString(TRW("none")), Configuration::ProxyType::None);
    m_proxyType.SetItemData(m_proxyType.AddString(TRW("http")), Configuration::ProxyType::HTTP);
    m_proxyType.SetItemData(m_proxyType.AddString(TRW("http_with_credentials")), Configuration::ProxyType::HTTP_Password);
    m_proxyType.SetItemData(m_proxyType.AddString(TRW("i2p")), Configuration::ProxyType::I2P);
    m_proxyType.SetItemData(m_proxyType.AddString(TRW("socks4")), Configuration::ProxyType::SOCKS4);
    m_proxyType.SetItemData(m_proxyType.AddString(TRW("socks5")), Configuration::ProxyType::SOCKS5);
    m_proxyType.SetItemData(m_proxyType.AddString(TRW("socks5_with_credentials")), Configuration::ProxyType::SOCKS5_Password);

    SelectProxyType(cfg.GetProxyType());
    m_proxyHost.SetWindowText(TWS(cfg.GetProxyHost()));
    if (cfg.GetProxyPort() > 0) { m_proxyPort.SetWindowText(TWS(std::to_string(cfg.GetProxyPort()))); }
    m_proxyUsername.SetWindowText(TWS(cfg.GetProxyUsername()));
    m_proxyPassword.SetWindowText(TWS(cfg.GetProxyPassword()));
    if (cfg.GetProxyForce()) { m_proxyForce.SetCheck(BST_CHECKED); }
    if (cfg.GetProxyHostnames()) { m_proxyHostnames.SetCheck(BST_CHECKED); }
    if (cfg.GetProxyPeers()) { m_proxyPeers.SetCheck(BST_CHECKED); }
    if (cfg.GetProxyTrackers()) { m_proxyTrackers.SetCheck(BST_CHECKED); }

    return FALSE;
}

void ConnectionPage::SelectProxyType(int type)
{
    for (int i = 0; i < m_proxyType.GetCount(); i++)
    {
        LRESULT data = m_proxyType.GetItemData(i);

        if (data == type)
        {
            m_proxyType.SetCurSel(i);
            break;
        }
    }
}
