#include <picotorrent/client/ui/property_sheets/preferences/remote_page.hpp>

#include <sstream>

#include <windowsx.h>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/qr/qr_code.hpp>
#include <picotorrent/client/ui/dialogs/remote_qr_dialog.hpp>
#include <picotorrent/client/ui/resources.hpp>

using picotorrent::client::qr::qr_code;
using picotorrent::client::ui::dialogs::remote_qr_dialog;
using picotorrent::client::ui::property_sheets::preferences::remote_page;

remote_page::remote_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_PREFERENCES_REMOTE);
    set_title(TR("remote"));

    edit_font_ = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Consolas"));
}

remote_page::~remote_page()
{
    DeleteObject(edit_font_);
}

bool remote_page::enable_websocket_api()
{
    return IsDlgButtonChecked(handle(), ID_REMOTE_ENABLE) == BST_CHECKED;
}

void remote_page::set_certificate_public_key(const std::string &pubkey)
{
    set_dlg_item_text(ID_REMOTE_CERT_PUBKEY, pubkey);
}

void remote_page::set_enable_websocket_api(bool value)
{
    UINT check = value ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(handle(), ID_REMOTE_ENABLE, check);
}

void remote_page::set_websocket_access_token(const std::string &token)
{
    set_dlg_item_text(ID_REMOTE_TOKEN, token);
}

void remote_page::set_websocket_port(int port)
{
    set_dlg_item_text(ID_REMOTE_PORT, std::to_string(port));
}

int remote_page::websocket_port()
{
    std::string text = get_dlg_item_text(ID_REMOTE_PORT);
    if (text.empty()) { return -1; }
    return std::stoi(text);
}

BOOL remote_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    if (!is_initializing())
    {
        check_changed(hDlg, uCtrlId, HIWORD(wParam));
    }

    switch (uCtrlId)
    {
    case ID_REMOTE_ENABLE:
    {
        bool checked = IsDlgButtonChecked(hDlg, ID_REMOTE_ENABLE) == BST_CHECKED;
        CheckDlgButton(hDlg, ID_REMOTE_ENABLE, checked ? BST_UNCHECKED : BST_CHECKED);

        remote_qr_dialog dlg;
        dlg.show_modal(handle());

        break;
    }
    }

    switch (HIWORD(wParam))
    {
    case EN_SETFOCUS:
    {
        if (uCtrlId == ID_REMOTE_TOKEN
            || uCtrlId == ID_REMOTE_CERT_PUBKEY)
        {
            HWND hCtrl = GetDlgItem(handle(), uCtrlId);
            Edit_SetSel(hCtrl, 0, -1);
        }
        break;
    }
    }

    return FALSE;
}

void remote_page::on_init_dialog()
{
    set_dlg_item_text(ID_REMOTE_OPTIONS_GROUP, TR("options"));
    set_dlg_item_text(ID_REMOTE_ENABLE, TR("enable_websocket_api"));
    set_dlg_item_text(ID_REMOTE_PORT_TEXT, TR("port"));
    set_dlg_item_text(ID_REMOTE_SECURITY_GROUP, TR("security"));
    set_dlg_item_text(ID_REMOTE_TOKEN_TEXT, TR("access_token"));
    set_dlg_item_text(ID_REMOTE_CERT_TEXT, TR("certificate_public_key"));

    SendMessage(
        GetDlgItem(handle(), ID_REMOTE_TOKEN),
        WM_SETFONT,
        (WPARAM)edit_font_,
        0);

    SendMessage(
        GetDlgItem(handle(), ID_REMOTE_CERT_PUBKEY),
        WM_SETFONT,
        (WPARAM)edit_font_,
        0);
}

void remote_page::check_changed(HWND hDlg, UINT uCtrlId, UINT uCommand)
{
    switch (uCtrlId)
    {
    case ID_REMOTE_ENABLE:
    case ID_REMOTE_PORT:
    {
        switch (uCommand)
        {
        case BN_CLICKED:
        case CBN_SELENDOK:
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
