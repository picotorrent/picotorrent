#include <picotorrent/client/ui/property_sheets/preferences/downloads_page.hpp>

#include <picotorrent/core/filesystem/path.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/open_file_dialog.hpp>
#include <picotorrent/client/ui/resources.hpp>

namespace fs = picotorrent::core::filesystem;
using picotorrent::client::ui::property_sheets::preferences::downloads_page;
using picotorrent::client::ui::open_file_dialog;

const GUID DLG_BROWSE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x62 } };

downloads_page::downloads_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_PREFERENCES_DOWNLOADS);
    set_title(TR("downloads"));
}

std::wstring downloads_page::downloads_path()
{
    TCHAR p[MAX_PATH];
    GetDlgItemText(handle(), ID_PREFS_DEFSAVEPATH, p, MAX_PATH);
    return p;
}

bool downloads_page::prompt_for_save_path()
{
    return IsDlgButtonChecked(handle(), ID_PREFS_PROMPTFORSAVEPATH) == BST_CHECKED;
}

void downloads_page::set_downloads_path(const std::wstring &path)
{
    SetDlgItemText(handle(), ID_PREFS_DEFSAVEPATH, path.c_str());
}

void downloads_page::set_prompt_for_save_path(bool prompt)
{
    CheckDlgButton(handle(), ID_PREFS_PROMPTFORSAVEPATH, prompt ? BST_CHECKED : BST_UNCHECKED);
}

BOOL downloads_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    switch (uCtrlId)
    {
    case ID_PREFS_DEFSAVEPATH:
        if (HIWORD(wParam) == EN_CHANGE && !is_initializing())
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
        }
        break;

    case ID_PREFS_DEFSAVEPATH_BROWSE:
    {
        TCHAR p[MAX_PATH];
        GetDlgItemText(hDlg, ID_PREFS_DEFSAVEPATH, p, MAX_PATH);

        open_file_dialog dlg;
        dlg.set_guid(DLG_BROWSE);
        dlg.set_folder(p);
        dlg.set_options(dlg.options() | FOS_PICKFOLDERS);
        dlg.set_title(TEXT("Choose save path"));

        dlg.show(handle());

        std::vector<fs::path> paths = dlg.get_paths();

        if (paths.size() > 0)
        {
            SetDlgItemText(hDlg, ID_PREFS_DEFSAVEPATH, paths[0].to_string().c_str());
        }

        break;
    }

    case ID_PREFS_PROMPTFORSAVEPATH:
    {
        bool checked = IsDlgButtonChecked(hDlg, ID_PREFS_PROMPTFORSAVEPATH) == BST_CHECKED;
        CheckDlgButton(hDlg, ID_PREFS_PROMPTFORSAVEPATH, checked ? BST_UNCHECKED : BST_CHECKED);

        if (!is_initializing())
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
        }
        break;
    }
    }

    return FALSE;
}

void downloads_page::on_init_dialog()
{
    SetDlgItemText(handle(), ID_TRANSFERS_GROUP, TR("transfers"));
    SetDlgItemText(handle(), ID_DEFSAVEPATH_TEXT, TR("path"));
    SetDlgItemText(handle(), ID_PREFS_DEFSAVEPATH_BROWSE, TR("browse"));
    SetDlgItemText(handle(), ID_PREFS_PROMPTFORSAVEPATH, TR("prompt_for_save_path"));
    SetDlgItemText(handle(), ID_DL_PREFS_LIMITS_GROUP, TR("global_limits"));
    SetDlgItemText(handle(), ID_PREFS_GLOBAL_DL_LIMIT_TEXT, TR("dl_limit"));
    SetDlgItemText(handle(), ID_PREFS_GLOBAL_UL_LIMIT_TEXT, TR("ul_limit"));
}
