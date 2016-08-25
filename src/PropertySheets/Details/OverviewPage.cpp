#include "OverviewPage.hpp"

#include <strsafe.h>

#include "../../resources.h"
#include "../../Configuration.hpp"
#include "../../Translator.hpp"

using PropertySheets::Details::OverviewPage;

OverviewPage::OverviewPage()
{
    m_title = TRW("overview");
    SetTitle(m_title.c_str());
}

void OverviewPage::OnDestroy()
{
    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_UNREGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);
}

BOOL OverviewPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_REGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);

    return FALSE;
}

LRESULT OverviewPage::OnFoo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}
