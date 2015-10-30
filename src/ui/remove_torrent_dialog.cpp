#include <picotorrent/ui/remove_torrent_dialog.hpp>

#include <commctrl.h>

using picotorrent::ui::remove_torrent_dialog;

remove_torrent_dialog::remove_torrent_dialog()
{
}

remove_torrent_dialog::~remove_torrent_dialog()
{
}

int remove_torrent_dialog::show(HWND parent)
{
    TASKDIALOG_BUTTON buttons[] = 
    {
        { ID_REMOVE_DATA, TEXT("Yes, remove the data") },
        { ID_KEEP_DATA, TEXT("No, keep the data") }
    };

    TASKDIALOGCONFIG config = { 0 };
    config.cbSize = sizeof(config);
    config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    config.dwFlags = TDF_USE_COMMAND_LINKS;
    config.hwndParent = parent;
    config.pszWindowTitle = TEXT("PicoTorrent");
    config.pszMainIcon = TD_INFORMATION_ICON;
    config.pszMainInstruction = TEXT("Would you also like to remove the data files?");

    config.cButtons = ARRAYSIZE(buttons);
    config.pButtons = buttons;

    int btn;
    HRESULT hRes = TaskDialogIndirect(&config, &btn, NULL, NULL);

    return btn;
}
