#pragma once

#include <string>
#include "../stdafx.h"

namespace pico
{
    class RenameFileDialog : public CDialogImpl<RenameFileDialog>
    {
    public:
        enum { IDD = IDD_RENAMEFILE };

        RenameFileDialog(std::wstring currentName);

        std::wstring GetValue();

    private:
        LRESULT OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

        BEGIN_MSG_MAP(RenameFileDialog)
            COMMAND_ID_HANDLER(IDCANCEL, OnClose)
            COMMAND_ID_HANDLER(IDOK, OnClose)
            MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        END_MSG_MAP()

        std::wstring currentName_;
    };
}
