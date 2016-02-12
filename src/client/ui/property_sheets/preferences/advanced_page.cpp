#include <picotorrent/client/ui/property_sheets/preferences/advanced_page.hpp>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>

using picotorrent::client::ui::property_sheets::preferences::advanced_page;

advanced_page::advanced_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_PREFERENCES_ADVANCED);
    set_title(TR("advanced"));
}

bool advanced_page::use_picotorrent_id()
{
    return IsDlgButtonChecked(handle(), ID_USE_PICO_ID) == BST_CHECKED;
}

void advanced_page::set_use_picotorrent_id(bool value)
{
    UINT check = value ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(handle(), ID_USE_PICO_ID, check);
}

BOOL advanced_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    switch (uCtrlId)
    {
    case ID_USE_PICO_ID:
    {
        bool checked = IsDlgButtonChecked(hDlg, ID_USE_PICO_ID) == BST_CHECKED;
        CheckDlgButton(hDlg, ID_USE_PICO_ID, checked ? BST_UNCHECKED : BST_CHECKED);

        if (!is_initializing())
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
        }
        break;
    }
    }

    return FALSE;
}

void advanced_page::on_init_dialog()
{
    SetDlgItemText(handle(), ID_EXPERIMENTAL_GROUP, TR("experimental"));
    SetDlgItemText(handle(), ID_USE_PICO_ID, TR("use_picotorrent_peer_id"));
}
