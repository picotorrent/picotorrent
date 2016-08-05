#include <picotorrent/client/ui/property_sheets/details/overview_page.hpp>

#include <iomanip>
#include <sstream>

#include <shlwapi.h>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/common/string_operations.hpp>

using picotorrent::client::ui::property_sheets::details::overview_page;
using picotorrent::common::to_string;

overview_page::overview_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_OVERVIEW);
    set_title(TR("overview"));
}

void overview_page::set_piece_info(int count, int have, int length)
{
    std::stringstream ss;
    ss << count << " x " << bytes_to_string(length) << " (have " << have << ")";
    set_dlg_item_text(ID_OVERVIEW_PIECES, ss.str());
}

void overview_page::set_ratio(float ratio)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << ratio;
    set_dlg_item_text(ID_OVERVIEW_RATIO, ss.str());
}

void overview_page::set_total_download(int64_t dl)
{
    set_dlg_item_text(ID_OVERVIEW_DOWNLOADED, bytes_to_string(dl));
}

void overview_page::set_total_upload(int64_t dl)
{
    set_dlg_item_text(ID_OVERVIEW_UPLOADED, bytes_to_string(dl));
}

BOOL overview_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void overview_page::on_init_dialog()
{
    set_dlg_item_text(ID_OVERVIEW_STATISTICS_GROUP, TR("statistics"));
    set_dlg_item_text(ID_OVERVIEW_RATIO_TEXT, TR("ratio"));
    set_dlg_item_text(ID_OVERVIEW_PIECES_TEXT, TR("pieces"));
    set_dlg_item_text(ID_OVERVIEW_DOWNLOADED_TEXT, TR("downloaded"));
    set_dlg_item_text(ID_OVERVIEW_UPLOADED_TEXT, TR("uploaded"));
}

std::string overview_page::bytes_to_string(int64_t b)
{
    TCHAR size_str[100];
    StrFormatByteSize64(
        b,
        size_str,
        ARRAYSIZE(size_str));
    return to_string(size_str);
}
