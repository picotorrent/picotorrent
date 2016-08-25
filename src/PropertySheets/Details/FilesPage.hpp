#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <memory>
#include <string>
#include <vector>

namespace libtorrent
{
    struct torrent_handle;
}

namespace Models
{
    struct TorrentFile;
}

namespace UI
{
    class TorrentFileListView;
}

namespace PropertySheets
{
namespace Details
{
    class FilesPage : public CPropertyPageImpl<FilesPage>
    {
        friend class CPropertyPageImpl<FilesPage>;

    public:
        enum { IDD = IDD_DETAILS_FILES };
        FilesPage(const libtorrent::torrent_handle& th);

    private:
        std::vector<Models::TorrentFile> Map(const libtorrent::torrent_handle& th);
        void OnDestroy();
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
        LRESULT OnPrioritizeFiles(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnTorrentUpdated(UINT uMsg, WPARAM wParam, LPARAM lParam);

        BEGIN_MSG_MAP_EX(FilesPage)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_INITDIALOG(OnInitDialog)

            MESSAGE_HANDLER_EX(PT_PRIORITIZEFILES, OnPrioritizeFiles)
            MESSAGE_HANDLER_EX(PT_TORRENT_UPDATED, OnTorrentUpdated)

            CHAIN_MSG_MAP(CPropertyPageImpl<FilesPage>)
        END_MSG_MAP()

        std::wstring m_title;
        std::unique_ptr<UI::TorrentFileListView> m_filesList;
        const libtorrent::torrent_handle& m_torrent;
    };
}
}
