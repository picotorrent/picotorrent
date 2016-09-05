#pragma once

#include <string>

#include "../../resources.h"
#include "../../stdafx.h"
#include "../../UI/CheckBox.hpp"
#include "../../UI/TextBox.hpp"

namespace libtorrent
{
    struct torrent_handle;
}

namespace PropertySheets
{
namespace Details
{
    class OptionsPage : public CPropertyPageImpl<OptionsPage>
    {
        friend class CPropertyPageImpl<OptionsPage>;

    public:
        enum { IDD = IDD_DETAILS_OPTIONS };
        OptionsPage(const libtorrent::torrent_handle& torrent);

    protected:
        BOOL OnApply();

    private:
        void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

        BEGIN_MSG_MAP_EX(OptionsPage)
            MSG_WM_COMMAND(OnCommand)
            MSG_WM_INITDIALOG(OnInitDialog)
            CHAIN_MSG_MAP(CPropertyPageImpl<OptionsPage>)
        END_MSG_MAP()

        std::wstring m_title;

        UI::TextBox m_dlLimit;
        UI::TextBox m_ulLimit;
        UI::TextBox m_maxConnections;
        UI::TextBox m_maxUploads;
        UI::CheckBox m_sequentialDownload;

        const libtorrent::torrent_handle& m_torrent;
    };
}
}
