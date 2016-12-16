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

void ConnectionPage::ChangeProxy(int type)
{
    m_proxyHost.EnableWindow(FALSE);
    m_proxyPort.EnableWindow(FALSE);
    m_proxyUsername.EnableWindow(FALSE);
    m_proxyPassword.EnableWindow(FALSE);
    m_proxyForce.EnableWindow(FALSE);
    m_proxyHostnames.EnableWindow(FALSE);
    m_proxyPeers.EnableWindow(FALSE);
    m_proxyTrackers.EnableWindow(FALSE);

    switch (type)
    {
    case Configuration::ProxyType::SOCKS4:
    case Configuration::ProxyType::SOCKS5:
    case Configuration::ProxyType::SOCKS5_Password:
    case Configuration::ProxyType::HTTP:
    case Configuration::ProxyType::HTTP_Password:
    case Configuration::ProxyType::I2P:
    {
        m_proxyHost.EnableWindow(TRUE);
        m_proxyPort.EnableWindow(TRUE);
        m_proxyForce.EnableWindow(TRUE);
        m_proxyPeers.EnableWindow(TRUE);
        m_proxyTrackers.EnableWindow(TRUE);
        m_proxyHostnames.EnableWindow(TRUE);
        break;
    }
    }

    switch (type)
    {
    case Configuration::ProxyType::SOCKS5_Password:
    case Configuration::ProxyType::HTTP_Password:
    {
        m_proxyUsername.EnableWindow(TRUE);
        m_proxyPassword.EnableWindow(TRUE);
        break;
    }
    }
}

std::vector<std::pair<std::string, int>> ConnectionPage::GetListenInterfaces()
{
    std::vector<std::pair<std::string, int>> result;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    std::string l = m_listenInterfaces.GetValueA();

    while ((pos = l.find('\n', prev)) != std::string::npos)
    {
        std::string net_addr = Trim(l.substr(prev, pos - prev));
        size_t idx = net_addr.find_last_of(":");

        if (idx == std::string::npos)
        {
            continue;
        }

        std::string addr = net_addr.substr(0, idx);
        int port = std::stoi(net_addr.substr(idx + 1));

        result.push_back({ addr,port });
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    std::string last_net_addr = Trim(l.substr(prev, pos - prev));
    size_t lidx = last_net_addr.find_last_of(":");

    if (lidx != std::string::npos)
    {
        std::string addr = last_net_addr.substr(0, lidx);
        int port = std::stoi(last_net_addr.substr(lidx + 1));
        result.push_back({ addr,port });
    }

    return result;
}

BOOL ConnectionPage::OnApply()
{
    Configuration& cfg = Configuration::GetInstance();

    cfg.SetListenInterfaces(GetListenInterfaces());
    cfg.SetProxyType(m_proxyType.GetSelectedItemData<Configuration::ProxyType>());
    cfg.SetProxyHost(m_proxyHost.GetValueA());

    std::string port = m_proxyPort.GetValueA();
    cfg.SetProxyPort(port.empty() ? -1 : std::stoi(port));

    cfg.SetProxyUsername(m_proxyUsername.GetValueA());
    cfg.SetProxyPassword(m_proxyPassword.GetValueA());
    cfg.SetProxyForce(m_proxyForce.IsChecked());
    cfg.SetProxyHostnames(m_proxyHostnames.IsChecked());
    cfg.SetProxyPeers(m_proxyPeers.IsChecked());
    cfg.SetProxyTrackers(m_proxyTrackers.IsChecked());

    return TRUE;
}

void ConnectionPage::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    switch (nID)
    {
    case ID_LISTEN_INTERFACES:
        if (uNotifyCode == EN_CHANGE) { SetModified(); }
        break;
    case ID_PROXY_TYPE:
        if (uNotifyCode == CBN_SELENDOK) { SetModified(); }
        ChangeProxy(m_proxyType.GetSelectedItemData<Configuration::ProxyType>());
        break;
    case ID_PROXY_HOST:
    case ID_PROXY_PORT:
    case ID_PROXY_USERNAME:
    case ID_PROXY_PASSWORD:
        if (uNotifyCode == EN_CHANGE) { SetModified(); }
        break;
    case ID_PROXY_FORCE:
    case ID_PROXY_HOSTNAMES:
    case ID_PROXY_PEERS:
    case ID_PROXY_TRACKERS:
        UI::CheckBox cb = GetDlgItem(nID);
        cb.Toggle();
        SetModified();
        break;
    }
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

    // Checkboxes
    if (cfg.GetProxyForce()) { m_proxyForce.Check(); }
    if (cfg.GetProxyHostnames()) { m_proxyHostnames.Check(); }
    if (cfg.GetProxyPeers()) { m_proxyPeers.Check(); }
    if (cfg.GetProxyTrackers()) { m_proxyTrackers.Check(); }

    ChangeProxy(cfg.GetProxyType());

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
