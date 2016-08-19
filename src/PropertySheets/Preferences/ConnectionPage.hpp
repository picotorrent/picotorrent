#pragma once

#include "../../resources.h"
#include "../../stdafx.h"
#include "../../UI/CheckBox.hpp"
#include "../../UI/ComboBox.hpp"
#include "../../UI/TextBox.hpp"

#include <string>
#include <vector>

namespace PropertySheets
{
namespace Preferences
{
    class ConnectionPage : public CPropertyPageImpl<ConnectionPage>
    {
        friend class CPropertyPageImpl<ConnectionPage>;

    public:
        enum { IDD = IDD_PREFERENCES_CONNECTION };
        ConnectionPage();

    protected:
        BOOL OnApply();

    private:
        void ChangeProxy(int type);
        std::vector<std::pair<std::string, int>> GetListenInterfaces();

        void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
        void SelectProxyType(int type);

        BEGIN_MSG_MAP_EX(ConnectionPage)
            MSG_WM_COMMAND(OnCommand)
            MSG_WM_INITDIALOG(OnInitDialog)
            CHAIN_MSG_MAP(CPropertyPageImpl<ConnectionPage>)
        END_MSG_MAP()

        std::wstring m_title;

        UI::TextBox m_listenInterfaces;
        UI::ComboBox m_proxyType;
        UI::TextBox m_proxyHost;
        UI::TextBox m_proxyPort;
        UI::TextBox m_proxyUsername;
        UI::TextBox m_proxyPassword;
        UI::CheckBox m_proxyForce;
        UI::CheckBox m_proxyHostnames;
        UI::CheckBox m_proxyPeers;
        UI::CheckBox m_proxyTrackers;
    };
}
}
