#include <picotorrent/ui/property_sheets/details/files_page.hpp>

#include <picotorrent/ui/resources.hpp>

using picotorrent::ui::property_sheets::details::files_page;

files_page::files_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_FILES);
    set_title_id(IDS_DETAILS_FILES_TITLE);
}

BOOL files_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}
