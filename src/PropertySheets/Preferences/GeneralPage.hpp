#pragma once

#include "../../resources.h"
#include "../../stdafx.h"
#include "../../UI/CheckBox.hpp"

#include <memory>
#include <string>

namespace UI
{
    class NotifyIcon;
}

namespace PropertySheets
{
namespace Preferences
{
    class GeneralPage : public CPropertyPageImpl<GeneralPage>
    {
        friend class CPropertyPageImpl<GeneralPage>;

    public:
        enum { IDD = IDD_PREFERENCES_GENERAL };
        GeneralPage(const std::shared_ptr<UI::NotifyIcon>& notifyIcon);

    protected:
        BOOL OnApply();

    private:
        void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
        void SelectComboBoxItemWithData(CComboBox& cb, int data);
        void UpdateNotificationState(bool checked);

        BEGIN_MSG_MAP_EX(GeneralPage)
            MSG_WM_COMMAND(OnCommand)
            MSG_WM_INITDIALOG(OnInitDialog)
            CHAIN_MSG_MAP(CPropertyPageImpl<GeneralPage>)
        END_MSG_MAP()

        std::shared_ptr<UI::NotifyIcon> m_notifyIcon;

        std::wstring m_title;
        CComboBox m_languages;
        CComboBox m_startPosition;
        CButton m_autoStart;
        UI::CheckBox m_showNotificationIcon;
        UI::CheckBox m_minimizeToTray;
        UI::CheckBox m_closeToTray;
    };
}
}
