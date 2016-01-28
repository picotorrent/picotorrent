#include <picotorrent/ui/property_sheets/details/peers_page.hpp>

#include <picotorrent/ui/controls/list_view.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <shlwapi.h>
#include <strsafe.h>

#define LIST_COLUMN_IP 1
#define LIST_COLUMN_CLIENT 2
#define LIST_COLUMN_FLAGS 3
#define LIST_COLUMN_DOWNLOAD 4
#define LIST_COLUMN_UPLOAD 5

using picotorrent::ui::controls::list_view;
using picotorrent::ui::property_sheets::details::peers_page;
using picotorrent::ui::scaler;

peers_page::peers_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_PEERS);
    set_title_id(IDS_DETAILS_PEERS_TITLE);
}

peers_page::~peers_page()
{
}

void peers_page::add_peer(const std::string &pid, const std::wstring &ip, int port)
{
    peer_item pi;
    pi.pid = pid;
    pi.ip = ip;
    pi.port = port;

    peers_.push_back(pi);

    HWND hPeersList = GetDlgItem(handle(), ID_DETAILS_PEERS_LIST);
    ListView_SetItemCount(hPeersList, (int)peers_.size());
} 

void peers_page::begin_update()
{
    for (peer_item &item : peers_)
    {
        item.clean = true;
    }
}

void peers_page::end_update()
{
    peers_.erase(std::remove_if(peers_.begin(), peers_.end(),
        [] (const peer_item &item)
    {
        return item.clean;
    }), peers_.end());

    HWND hPeersList = GetDlgItem(handle(), ID_DETAILS_PEERS_LIST);
    ListView_SetItemCount(hPeersList, (int)peers_.size());
}

bool peers_page::has_peer(const std::string &pid)
{
    auto item = std::find_if(peers_.begin(), peers_.end(), [pid](const peer_item &i) { return i.pid == pid; });
    return item != peers_.end();
}

void peers_page::update_peer(const std::string &pid, const std::wstring &client, const std::wstring &flags, int dl, int ul)
{
    auto &item = std::find_if(peers_.begin(), peers_.end(), [pid](const peer_item &i) { return i.pid == pid; });
    item->client = client;
    item->flags = flags;
    item->dl = dl;
    item->ul = ul;
    item->clean = false;
}

BOOL peers_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void peers_page::on_init_dialog()
{
    HWND hList = GetDlgItem(handle(), ID_DETAILS_PEERS_LIST);
    list_ = std::make_unique<list_view>(hList);

    list_->add_column(LIST_COLUMN_IP,       L"IP",     140);
    list_->add_column(LIST_COLUMN_CLIENT,   L"Client", 80);
    list_->add_column(LIST_COLUMN_FLAGS,    L"Flags",  80);
    list_->add_column(LIST_COLUMN_DOWNLOAD, L"DL",     140, list_view::number);
    list_->add_column(LIST_COLUMN_UPLOAD,   L"UL",     140, list_view::number);

    list_->on_display().connect(std::bind(&peers_page::on_list_display, this, std::placeholders::_1));
}

std::wstring peers_page::on_list_display(int id)
{
    switch (id)
    {
    case LIST_COLUMN_IP:
        return L"IP";
    default:
        return L"<unknown column>";
    }
}
