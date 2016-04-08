#include <picotorrent/client/ui/property_sheets/preferences/general_page.hpp>

#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>

#include <windowsx.h>

namespace i18n = picotorrent::client::i18n;
using picotorrent::client::ui::property_sheets::preferences::general_page;

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
        int index = ComboBox_AddString(hCombo, to_wstring(translation.name).c_str());
        ComboBox_SetItemData(hCombo, index, translation.language_id);
    }
}

void general_page::add_start_position(int id, const std::string &name)
{
    HWND hCombo = GetDlgItem(handle(), ID_START_POSITION);

    int index = ComboBox_AddString(hCombo, to_wstring(name).c_str());
    ComboBox_SetItemData(hCombo, index, id);
}

bool general_page::get_autostart_checked()
{
    return IsDlgButtonChecked(handle(), ID_AUTOSTART_PICO) == BST_CHECKED;
}

int general_page::get_selected_language()
{
    HWND ctl = GetDlgItem(handle(), ID_LANGUAGE);
    return (int)ComboBox_GetItemData(ctl, ComboBox_GetCurSel(ctl));
}

int general_page::get_selected_start_position()
{
    HWND ctl = GetDlgItem(handle(), ID_START_POSITION);
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

void general_page::select_start_position(int posId)
{
    HWND hCombo = GetDlgItem(handle(), ID_START_POSITION);

    for (int i = 0; i < ComboBox_GetCount(hCombo); i++)
    {
        LRESULT data = ComboBox_GetItemData(hCombo, i);

        if (data == posId)
        {
            ComboBox_SetCurSel(hCombo, i);
            break;
        }
    }
}

void general_page::set_autostart_checked(bool v)
{
    CheckDlgButton(handle(), ID_AUTOSTART_PICO, v ? BST_CHECKED : BST_UNCHECKED);
}

BOOL general_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    switch (uCtrlId)
    {
    case ID_LANGUAGE:
    case ID_START_POSITION:
    {
        if (!is_initializing() && HIWORD(wParam) == CBN_SELENDOK)
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
        }
        break;
    }
    case ID_AUTOSTART_PICO:
    {
        if (!is_initializing())
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
            // Toggle the checkbox
            set_autostart_checked(!get_autostart_checked());
        }
        break;
    }
    }

    return FALSE;
}

void general_page::on_init_dialog()
{
    set_dlg_item_text(ID_UI_GROUP, TR("user_interface"));
    set_dlg_item_text(ID_LANGUAGE_TEXT, TR("language"));
    set_dlg_item_text(ID_MISC_GROUP, TR("miscellaneous"));
    set_dlg_item_text(ID_AUTOSTART_PICO, TR("start_with_windows"));
    set_dlg_item_text(ID_START_POSITION_TEXT, TR("start_position"));
}
