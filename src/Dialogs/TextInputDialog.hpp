#pragma once

#include <string>

#include "../resources.h"
#include "../stdafx.h"

namespace Dialogs
{
    class TextInputDialog : public CDialogImpl<TextInputDialog>
    {
    public:
        enum { IDD = IDD_TEXTINPUT };

        std::wstring GetInput();
        void SetOkText(std::wstring const& text);
        void SetTitle(std::wstring const& title);

    private:
        void OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl);
        BOOL OnInitDialog(CWindow wnd, LPARAM lParam);
        void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl);

        BEGIN_MSG_MAP(TextInputDialog)
            MSG_WM_INITDIALOG(OnInitDialog)
            COMMAND_ID_HANDLER_EX(IDCANCEL, OnEndDialog)
            COMMAND_ID_HANDLER_EX(IDOK, OnOk)
        END_MSG_MAP()

        std::wstring m_input;
        std::wstring m_okText;
        std::wstring m_title;
    };
}
