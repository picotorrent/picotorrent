#include <picotorrent/ui/property_sheets/preferences/downloads_page.hpp>

#include <picotorrent/ui/resources.hpp>

using picotorrent::ui::property_sheets::preferences::downloads_page;

downloads_page::downloads_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(6767);
    set_title_id(IDS_PREFS_DOWNLOADS_TITLE);
}

void downloads_page::set_downloads_path(const std::wstring &path)
{
    SetWindowText(GetDlgItem(handle(), ID_PREFS_DEFSAVEPATH), path.c_str());
}
