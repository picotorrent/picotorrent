#include "OptionsPage.hpp"

#include <strsafe.h>

#include "../../resources.h"
#include "../../Configuration.hpp"
#include "../../Translator.hpp"

using PropertySheets::Details::OptionsPage;

OptionsPage::OptionsPage()
{
    m_title = TRW("options");
    SetTitle(m_title.c_str());
}

void OptionsPage::OnDestroy()
{
    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_UNREGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);
}

BOOL OptionsPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_REGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);

    return FALSE;
}
