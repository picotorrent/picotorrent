#include "PreviewPage.hpp"

#include <memory>

#include <atlctrls.h>

using Wizard::PreviewPage;

PreviewPage::PreviewPage()
{
    m_title = L"Preview torrents";
    SetHeaderTitle(m_title.c_str());
}

BOOL PreviewPage::OnSetActive()
{
    PropSheet_ShowWizButtons(m_hWnd, PSWIZB_NEXT, PSWIZB_NEXT);
    PropSheet_EnableWizButtons(m_hWnd, PSWIZB_NEXT, PSWIZB_NEXT);
    PropSheet_SetButtonText(m_hWnd, PSWIZB_NEXT, TEXT("Import"));

    return TRUE;
}
