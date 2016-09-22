#include "WelcomePage.hpp"

#include <memory>

#include <atlctrls.h>

using Wizard::WelcomePage;

WelcomePage::WelcomePage()
{
    m_title = L"Select application to import torrents from";
    SetHeaderTitle(m_title.c_str());
}

LRESULT WelcomePage::OnInitDialog(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return TRUE;
}

void WelcomePage::OnQBittorrentImport(UINT /*nofityCode*/, int /*commandId*/, HWND /*handle*/)
{
    GetPropertySheet().SetActivePageByID(IDD_WIZARD_CONFIGURE);
}

BOOL WelcomePage::OnSetActive()
{
    PropSheet_ShowWizButtons(m_hWnd, 0, PSWIZB_NEXT | PSWIZB_FINISH | PSWIZB_CANCEL);
    PropSheet_ShowWizButtons(m_hWnd, PSWIZB_CANCEL, PSWIZB_CANCEL);
    PropSheet_EnableWizButtons(m_hWnd, PSWIZB_CANCEL, PSWIZB_CANCEL);

    return TRUE;
}
