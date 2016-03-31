#include <picotorrent/client/ui/property_sheets/preferences/downloads_page.hpp>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/open_file_dialog.hpp>
#include <picotorrent/client/ui/resources.hpp>

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

std::string downloads_page::downloads_path()
{
    return get_dlg_item_text(ID_PREFS_DEFSAVEPATH);
}

int downloads_page::download_rate()
{
    return std::stoi(get_dlg_item_text(ID_PREFS_GLOBAL_DL_LIMIT));
}

bool downloads_page::prompt_for_save_path()
{
    return IsDlgButtonChecked(handle(), ID_PREFS_PROMPTFORSAVEPATH) == BST_CHECKED;
}
int downloads_page::upload_rate()
{
    return std::stoi(get_dlg_item_text(ID_PREFS_GLOBAL_UL_LIMIT));
}

void downloads_page::set_downloads_path(const std::string &path)
{
    set_dlg_item_text(ID_PREFS_DEFSAVEPATH, path);
}

void downloads_page::set_download_rate(int dl_rate)
{
    set_dlg_item_text(ID_PREFS_GLOBAL_DL_LIMIT, std::to_string(dl_rate));
}

void downloads_page::set_prompt_for_save_path(bool prompt)
{
    CheckDlgButton(handle(), ID_PREFS_PROMPTFORSAVEPATH, prompt ? BST_CHECKED : BST_UNCHECKED);
}

void downloads_page::set_upload_rate(int ul_rate)
{
    set_dlg_item_text(ID_PREFS_GLOBAL_UL_LIMIT, std::to_string(ul_rate));
}

BOOL downloads_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    switch (uCtrlId)
    {
    case ID_PREFS_GLOBAL_DL_LIMIT:
    case ID_PREFS_GLOBAL_UL_LIMIT:
    case ID_PREFS_DEFSAVEPATH:
        if (HIWORD(wParam) == EN_CHANGE && !is_initializing())
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
        }
        break;

    case ID_PREFS_DEFSAVEPATH_BROWSE:
    {
        open_file_dialog dlg;
        dlg.set_guid(DLG_BROWSE);
        dlg.set_folder(get_dlg_item_text(ID_PREFS_DEFSAVEPATH));
        dlg.set_options(dlg.options() | FOS_PICKFOLDERS);
        dlg.set_title("Choose save path");

        dlg.show(handle());

        std::vector<std::string> paths = dlg.get_paths();

        if (paths.size() > 0)
        {
            set_downloads_path(paths[0]);
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
    set_dlg_item_text(ID_TRANSFERS_GROUP, TR("transfers"));
    set_dlg_item_text(ID_DEFSAVEPATH_TEXT, TR("path"));
    set_dlg_item_text(ID_PREFS_DEFSAVEPATH_BROWSE, TR("browse"));
    set_dlg_item_text(ID_PREFS_PROMPTFORSAVEPATH, TR("prompt_for_save_path"));
    set_dlg_item_text(ID_DL_PREFS_LIMITS_GROUP, TR("global_limits"));
    set_dlg_item_text(ID_PREFS_GLOBAL_DL_LIMIT_TEXT, TR("dl_limit"));
    set_dlg_item_text(ID_PREFS_GLOBAL_DL_LIMIT_HELP, TR("dl_limit_help"));
    set_dlg_item_text(ID_PREFS_GLOBAL_UL_LIMIT_TEXT, TR("ul_limit"));
    set_dlg_item_text(ID_PREFS_GLOBAL_UL_LIMIT_HELP, TR("ul_limit_help"));
}
