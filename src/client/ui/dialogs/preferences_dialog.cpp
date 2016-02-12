#include <picotorrent/ui/dialogs/preferences_dialog.hpp>

#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/ui/open_file_dialog.hpp>
#include <picotorrent/ui/resources.hpp>

namespace fs = picotorrent::filesystem;
using picotorrent::ui::dialogs::preferences_dialog;

const GUID DLG_BROWSE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x62 } };

preferences_dialog::preferences_dialog()
    : dialog_base(IDD_PREFERENCES)
{
}

std::wstring preferences_dialog::default_save_path()
{
    return save_path_;
}

int preferences_dialog::listen_port()
{
    return listen_port_;
}

bool preferences_dialog::prompt_for_save_path()
{
    return prompt_;
}

void preferences_dialog::set_default_save_path(const std::wstring &path)
{
    save_path_ = path;
}

void preferences_dialog::set_listen_port(int port)
{
    listen_port_ = port;
}

void preferences_dialog::set_prompt_for_save_path(bool prompt)
{
    prompt_ = prompt;
}

BOOL preferences_dialog::on_command(int id, WPARAM wParam, LPARAM lParam)
{
    switch (id)
    {
    case IDCANCEL:
    case IDOK:
    {
        save_path_ = get_dlg_item_text(ID_PREFS_DEFSAVEPATH);
        prompt_ = is_dlg_button_checked(ID_PREFS_PROMPTFORSAVEPATH);
        listen_port_ = std::stoi(get_dlg_item_text(ID_PREFS_LISTENPORT));

        EndDialog(handle(), wParam);
        return TRUE;
    }

    case ID_PREFS_DEFSAVEPATH_BROWSE:
    {
        ui::open_file_dialog dlg;
        dlg.set_guid(DLG_BROWSE);
        dlg.set_folder(get_dlg_item_text(ID_PREFS_DEFSAVEPATH));
        dlg.set_options(dlg.options() | FOS_PICKFOLDERS);
        dlg.set_title(TEXT("Choose save path"));

        dlg.show(handle());

        std::vector<fs::path> paths = dlg.get_paths();

        if (paths.size() > 0)
        {
            set_dlg_item_text(ID_PREFS_DEFSAVEPATH, paths[0].to_string());
        }

        break;
    }
    
    case ID_PREFS_PROMPTFORSAVEPATH:
    {
        set_dlg_button_checked(
            ID_PREFS_PROMPTFORSAVEPATH,
            !is_dlg_button_checked(ID_PREFS_PROMPTFORSAVEPATH));
        break;
    }
    }

    return FALSE;
}

BOOL preferences_dialog::on_init_dialog()
{
    set_dlg_button_checked(ID_PREFS_PROMPTFORSAVEPATH, prompt_);
    set_dlg_item_text(ID_PREFS_DEFSAVEPATH, save_path_);
    set_dlg_item_text(ID_PREFS_LISTENPORT, std::to_wstring(listen_port_));

    return TRUE;
}
