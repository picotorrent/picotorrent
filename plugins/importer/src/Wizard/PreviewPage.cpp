#include "PreviewPage.hpp"

#include <memory>

#include <atlctrls.h>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <picotorrent/utils.hpp>

#include "../Sources/Source.hpp"
#include "WizardState.hpp"

namespace lt = libtorrent;
using Sources::Source;
using Wizard::PreviewPage;

PreviewPage::PreviewPage(std::shared_ptr<lt::session> session, std::shared_ptr<Wizard::WizardState> state)
    : m_state(state),
    m_session(session)
{
    m_title = L"Preview torrents";
    SetHeaderTitle(m_title.c_str());
}
LRESULT PreviewPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CListViewCtrl list(GetDlgItem(ID_LIST_PREVIEW));
    list.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    list.InsertColumn(0, TEXT("Name"), LVCFMT_LEFT, 220);
    list.InsertColumn(1, TEXT("Size"), LVCFMT_RIGHT, 80);
    list.InsertColumn(2, TEXT("Save path"), LVCFMT_LEFT, 280);

    return TRUE;
}

BOOL PreviewPage::OnSetActive()
{
    CListViewCtrl list(GetDlgItem(ID_LIST_PREVIEW));
    list.DeleteAllItems();

    int idx = 0;

    for (Source::AddTorrentRequest const& req : m_state->source->GetRequests())
    {
        TCHAR size[100];
        StrFormatByteSize64(req.ti->total_size(), size, ARRAYSIZE(size));

        idx = list.InsertItem(idx, TWS(req.ti->name()));
        list.SetItemText(idx, 1, size);
        list.SetItemText(idx, 2, TWS(req.savePath));
    }

    PropSheet_ShowWizButtons(
        m_hWnd,
        PSWIZB_CANCEL | PSWIZB_FINISH | PSWIZB_NEXT,
        PSWIZB_CANCEL | PSWIZB_FINISH);

    PropSheet_EnableWizButtons(
        m_hWnd,
        PSWIZB_CANCEL | PSWIZB_FINISH | PSWIZB_NEXT,
        PSWIZB_CANCEL | PSWIZB_FINISH);

    PropSheet_SetButtonText(m_hWnd, PSWIZB_FINISH, TEXT("Import"));

    return TRUE;
}

BOOL PreviewPage::OnWizardFinish()
{
    for (Source::AddTorrentRequest const& req : m_state->source->GetRequests())
    {
        lt::add_torrent_params p;
        p.file_priorities = req.filePriorities;
        p.save_path = req.savePath;
        p.ti = req.ti;

        m_session->async_add_torrent(p);
    }

    return TRUE;
}
