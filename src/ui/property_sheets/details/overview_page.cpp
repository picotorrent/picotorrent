#include <picotorrent/ui/property_sheets/details/overview_page.hpp>

#include <picotorrent/i18n/translator.hpp>
#include <picotorrent/ui/resources.hpp>

using picotorrent::ui::property_sheets::details::overview_page;

overview_page::overview_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_OVERVIEW);
    set_title(TR("overview"));
}

BOOL overview_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    if (!is_initializing())
    {
        check_changed(hDlg, uCtrlId, HIWORD(wParam));
    }

    switch (uCtrlId)
    {
    case ID_SEQUENTIAL_DOWNLOAD:
    {
        sequential_download(!sequential_download());
        break;
    }
    }

    return FALSE;
}

void overview_page::on_init_dialog()
{
    SetDlgItemText(handle(), ID_LIMITS_GROUP, TR("limits"));
    SetDlgItemText(handle(), ID_DL_LIMIT_TEXT, TR("dl_limit"));
    SetDlgItemText(handle(), ID_DL_LIMIT_HELP, TR("dl_limit_help"));
    SetDlgItemText(handle(), ID_UL_LIMIT_TEXT, TR("ul_limit"));
    SetDlgItemText(handle(), ID_UL_LIMIT_HELP, TR("ul_limit_help"));
    SetDlgItemText(handle(), ID_MAX_CONNECTIONS_TEXT, TR("max_connections"));
    SetDlgItemText(handle(), ID_MAX_CONNECTIONS_HELP, TR("max_connections_help"));
    SetDlgItemText(handle(), ID_MAX_UPLOADS_TEXT, TR("max_uploads"));
    SetDlgItemText(handle(), ID_MAX_UPLOADS_HELP, TR("max_uploads_help"));
    SetDlgItemText(handle(), ID_SEQUENTIAL_DOWNLOAD, TR("sequential_download"));
}

int overview_page::dl_limit()
{
    return std::stoi(get_dlg_item_text(ID_DL_LIMIT));
}

void overview_page::dl_limit(int limit)
{
    set_dlg_item_text(ID_DL_LIMIT, std::to_wstring(limit));
}

int overview_page::ul_limit()
{
    return std::stoi(get_dlg_item_text(ID_UL_LIMIT));
}

void overview_page::ul_limit(int limit)
{
    set_dlg_item_text(ID_UL_LIMIT, std::to_wstring(limit));
}

int overview_page::max_connections()
{
    return std::stoi(get_dlg_item_text(ID_MAX_CONNECTIONS));
}

void overview_page::max_connections(int max)
{
    set_dlg_item_text(ID_MAX_CONNECTIONS, std::to_wstring(max));
}

int overview_page::max_uploads()
{
    return std::stoi(get_dlg_item_text(ID_MAX_UPLOADS));
}

void overview_page::max_uploads(int max)
{
    set_dlg_item_text(ID_MAX_UPLOADS, std::to_wstring(max));
}

bool overview_page::sequential_download()
{
    return IsDlgButtonChecked(handle(), ID_SEQUENTIAL_DOWNLOAD) == BST_CHECKED;
}

void overview_page::sequential_download(bool val)
{
    UINT check = val ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(handle(), ID_SEQUENTIAL_DOWNLOAD, check);
}

void overview_page::check_changed(HWND hDlg, UINT uCtrlId, UINT uCommand)
{
    switch (uCtrlId)
    {
    case ID_DL_LIMIT:
    case ID_UL_LIMIT:
    case ID_MAX_CONNECTIONS:
    case ID_MAX_UPLOADS:
    case ID_SEQUENTIAL_DOWNLOAD:
    {
        switch (uCommand)
        {
        case BN_CLICKED:
        case EN_CHANGE:
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
            break;
        }
        }
        break;
    }
    }
}
