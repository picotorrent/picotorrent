#include <picotorrent/ui/remove_torrent_dialog.hpp>

#include <commctrl.h>

using picotorrent::ui::remove_torrent_dialog;

remove_torrent_dialog::remove_torrent_dialog()
{
}

remove_torrent_dialog::~remove_torrent_dialog()
{
}

int remove_torrent_dialog::show(HWND parent, bool remove_files)
{
    TASKDIALOG_BUTTON buttons[] = 
    {
        { ID_REMOVE_DATA, TEXT("Remove") }
    };

    TASKDIALOGCONFIG config = { 0 };
    config.cbSize = sizeof(config);
    config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    config.dwFlags = TDF_USE_COMMAND_LINKS;
    config.hwndParent = parent;
    config.pszWindowTitle = TEXT("PicoTorrent");

    if (remove_files)
    {
        config.pszMainIcon = TD_WARNING_ICON;
        config.pszMainInstruction = TEXT("The torrent and its files will be removed. Are you sure?");
    }
    else
    {
        config.pszMainIcon = TD_INFORMATION_ICON;
        config.pszMainInstruction = TEXT("The torrent will be removed, but its files will be kept on disk. Are you sure?");
    }


    config.cButtons = ARRAYSIZE(buttons);
    config.pButtons = buttons;

    int btn;
    HRESULT hRes = TaskDialogIndirect(&config, &btn, NULL, NULL);

    return btn;
}
