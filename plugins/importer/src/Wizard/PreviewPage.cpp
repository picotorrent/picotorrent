#include "PreviewPage.hpp"

#include <memory>

#include <atlctrls.h>

#include "../Sources/Source.hpp"
#include "WizardState.hpp"

using Sources::Source;
using Wizard::PreviewPage;

PreviewPage::PreviewPage(std::shared_ptr<Wizard::WizardState> state)
    : m_state(state)
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
    list.InsertColumn(2, TEXT("Save path"), LVCFMT_RIGHT, 280);

    return TRUE;
}

BOOL PreviewPage::OnSetActive()
{
    CListViewCtrl list(GetDlgItem(ID_LIST_PREVIEW));
    list.DeleteAllItems();

    int idx = 0;

    for (Source::PreviewItem const& item : m_state->source->GetPreview())
    {
        TCHAR size[100];
        StrFormatByteSize64(item.size, size, ARRAYSIZE(size));

        idx = list.InsertItem(idx, item.name.c_str());
        list.SetItemText(idx, 1, size);
        list.SetItemText(idx, 2, item.savePath.c_str());
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
