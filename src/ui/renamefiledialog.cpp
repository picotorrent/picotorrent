#include "renamefiledialog.h"

using namespace pico;

RenameFileDialog::RenameFileDialog(std::wstring currentName)
    : currentName_(currentName)
{
}

std::wstring RenameFileDialog::GetValue()
{
    return currentName_;
}

LRESULT RenameFileDialog::OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CEdit name = (CEdit)GetDlgItem(ID_RENAMEFILE_NAME);
    TCHAR buf[MAX_PATH];
    name.GetWindowTextW(buf, MAX_PATH);

    currentName_ = buf;

    EndDialog(wID);
    return FALSE;
}

LRESULT RenameFileDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CEdit name = (CEdit)GetDlgItem(ID_RENAMEFILE_NAME);
    name.SetWindowTextW(currentName_.c_str());

    CenterWindow(GetParent());

    return FALSE;
}
