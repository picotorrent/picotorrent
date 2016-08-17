#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <string>

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

    private:
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
        void SelectProxyType(int type);

        BEGIN_MSG_MAP_EX(ConnectionPage)
            MSG_WM_INITDIALOG(OnInitDialog)
            CHAIN_MSG_MAP(CPropertyPageImpl<ConnectionPage>)
        END_MSG_MAP()

        std::wstring m_title;

        CEdit m_listenInterfaces;
        CComboBox m_proxyType;
        CEdit m_proxyHost;
        CEdit m_proxyPort;
        CEdit m_proxyUsername;
        CEdit m_proxyPassword;
        CButton m_proxyForce;
        CButton m_proxyHostnames;
        CButton m_proxyPeers;
        CButton m_proxyTrackers;
    };
}
}
