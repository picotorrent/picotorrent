#include "TextInputDialog.hpp"

#include "../Translator.hpp"
#include "../UI/TextBox.hpp"

using Dialogs::TextInputDialog;

std::wstring TextInputDialog::GetInput()
{
    return m_input;
}

void TextInputDialog::SetOkText(std::wstring const& text)
{
    m_okText = text;
}

void TextInputDialog::SetTitle(std::wstring const& title)
{
    m_title = title;
}

void TextInputDialog::OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    EndDialog(nID);
}

BOOL TextInputDialog::OnInitDialog(CWindow wnd, LPARAM lParam)
{
    SetWindowText(m_title.c_str());
    SetDlgItemText(IDOK, m_okText.c_str());

    return FALSE;
}

void TextInputDialog::OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    UI::TextBox txt = GetDlgItem(ID_TEXTINPUT_FIELD);
    m_input = txt.GetValueW();
    EndDialog(IDOK);
}
