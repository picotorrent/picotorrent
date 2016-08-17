#pragma once

#include "../../stdafx.h"

#include <memory>

namespace PropertySheets
{
namespace Preferences
{
    class DownloadsPage;
    class GeneralPage;

    class PreferencesSheet : public CPropertySheetImpl<PreferencesSheet>
    {
        friend class CPropertySheetImpl<PreferencesSheet>;

    public:
        PreferencesSheet();
        ~PreferencesSheet();

    protected:
        void OnSheetInitialized();

    private:
        BEGIN_MSG_MAP_EX(PreferencesSheet)
            CHAIN_MSG_MAP(CPropertySheetImpl<PreferencesSheet>)
        END_MSG_MAP()

        std::unique_ptr<DownloadsPage> m_downloads;
        std::unique_ptr<GeneralPage> m_general;
    };
}
}
