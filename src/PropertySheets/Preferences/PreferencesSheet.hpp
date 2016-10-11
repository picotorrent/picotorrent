#pragma once

#include "../../stdafx.h"

#include <memory>

namespace UI
{
    class NotifyIcon;
}

namespace PropertySheets
{
namespace Preferences
{
    class ConnectionPage;
    class DownloadsPage;
    class GeneralPage;
    class PrivacyPage;

    class PreferencesSheet : public CPropertySheetImpl<PreferencesSheet>
    {
        friend class CPropertySheetImpl<PreferencesSheet>;

    public:
        PreferencesSheet(const std::shared_ptr<UI::NotifyIcon>& notifyIcon);
        ~PreferencesSheet();

    protected:
        void OnSheetInitialized();

    private:
        BEGIN_MSG_MAP_EX(PreferencesSheet)
            CHAIN_MSG_MAP(CPropertySheetImpl<PreferencesSheet>)
        END_MSG_MAP()

        std::unique_ptr<ConnectionPage> m_connection;
        std::unique_ptr<DownloadsPage> m_downloads;
        std::unique_ptr<GeneralPage> m_general;
        std::unique_ptr<PrivacyPage> m_privacy;
    };
}
}
