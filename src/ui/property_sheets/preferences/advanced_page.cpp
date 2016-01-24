#include <picotorrent/ui/property_sheets/preferences/advanced_page.hpp>

#include <picotorrent/ui/resources.hpp>

using picotorrent::ui::property_sheets::preferences::advanced_page;

advanced_page::advanced_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(6769);
    set_title_id(IDS_PREFS_ADVANCED_TITLE);
}

bool advanced_page::use_picotorrent_id()
{
    return IsDlgButtonChecked(handle(), ID_PREFS_ADV_USE_PICO_ID) == BST_CHECKED;
}

void advanced_page::set_use_picotorrent_id(bool value)
{
    UINT check = value ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(handle(), ID_PREFS_ADV_USE_PICO_ID, check);
}

BOOL advanced_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    switch (uCtrlId)
    {
    case ID_PREFS_ADV_USE_PICO_ID:
    {
        bool checked = IsDlgButtonChecked(hDlg, ID_PREFS_ADV_USE_PICO_ID) == BST_CHECKED;
        CheckDlgButton(hDlg, ID_PREFS_ADV_USE_PICO_ID, checked ? BST_UNCHECKED : BST_CHECKED);

        if (!is_initializing())
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
        }
        break;
    }
    }

    return FALSE;
}
