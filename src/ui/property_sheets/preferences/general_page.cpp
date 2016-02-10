#include <picotorrent/ui/property_sheets/preferences/general_page.hpp>

#include <picotorrent/i18n/translator.hpp>
#include <picotorrent/ui/resources.hpp>

#include <windowsx.h>

namespace i18n = picotorrent::i18n;
using picotorrent::ui::property_sheets::preferences::general_page;

general_page::general_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_PREFERENCES_GENERAL);
    set_title(TR("general"));
}

general_page::~general_page()
{
}

void general_page::add_languages(const std::vector<i18n::translation> &translations)
{
    HWND hCombo = GetDlgItem(handle(), ID_LANGUAGE);

    for (const i18n::translation &translation : translations)
    {
        int index = ComboBox_AddString(hCombo, translation.name.c_str());
        ComboBox_SetItemData(hCombo, index, translation.language_id);
    }
}

int general_page::get_selected_language()
{
    HWND ctl = GetDlgItem(handle(), ID_LANGUAGE);
    return (int)ComboBox_GetItemData(ctl, ComboBox_GetCurSel(ctl));
}

void general_page::select_language(int langId)
{
    HWND hCombo = GetDlgItem(handle(), ID_LANGUAGE);

    for (int i = 0; i < ComboBox_GetCount(hCombo); i++)
    {
        LRESULT data = ComboBox_GetItemData(hCombo, i);

        if (data == langId)
        {
            ComboBox_SetCurSel(hCombo, i);
            break;
        }
    }
}

BOOL general_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    switch (uCtrlId)
    {
    case ID_LANGUAGE:
    {
        if (!is_initializing() && HIWORD(wParam) == CBN_SELENDOK)
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
            break;
        }
    }
    }

    return FALSE;
}

void general_page::on_init_dialog()
{
    SetDlgItemText(handle(), ID_UI_GROUP, TR("user_interface"));
    SetDlgItemText(handle(), ID_LANGUAGE_TEXT, TR("language"));
}
