#include "QBittorrentPage.hpp"

#include <memory>

#include <atlctrls.h>

using Wizard::QBittorrentPage;

QBittorrentPage::QBittorrentPage()
{
    m_title = L"Import from qBittorrent";
    SetHeaderTitle(m_title.c_str());
}

LRESULT QBittorrentPage::OnInitDialog(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    PWSTR buf;
    if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &buf) != S_OK)
    {
        return TRUE;
    }

    TCHAR qb[MAX_PATH];
    PathCombine(qb, buf, TEXT("qBittorrent"));
    PathCombine(qb, qb, TEXT("BT_backup"));

    SetDlgItemText(1313, qb);

    CoTaskMemFree(buf);

    return TRUE;
}

BOOL QBittorrentPage::OnSetActive()
{
    PropSheet_ShowWizButtons(m_hWnd, PSWIZB_NEXT, PSWIZB_NEXT);
    PropSheet_EnableWizButtons(m_hWnd, PSWIZB_NEXT, PSWIZB_NEXT);
    PropSheet_SetButtonText(m_hWnd, PSWIZB_NEXT, TEXT("Preview"));

    return TRUE;
}

int QBittorrentPage::OnWizardNext()
{
    return IDD_WIZARD_PREVIEW;
}
