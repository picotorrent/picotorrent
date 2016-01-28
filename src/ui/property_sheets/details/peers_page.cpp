#include <picotorrent/ui/property_sheets/details/peers_page.hpp>

#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <shlwapi.h>
#include <strsafe.h>

using picotorrent::ui::property_sheets::details::peers_page;
using picotorrent::ui::scaler;

peers_page::peers_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_PEERS);
    set_title_id(IDS_DETAILS_PEERS_TITLE);
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
    HWND hPeersList = GetDlgItem(handle(), ID_DETAILS_PEERS_LIST);

    // Set style (full row select)
    ListView_SetExtendedListViewStyle(hPeersList, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    LVCOLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.pszText = L"Remote";
    col.cx = scaler::x(140);
    col.fmt = LVCFMT_LEFT;

    ListView_InsertColumn(hPeersList, 0, &col);

    col.pszText = L"Client";
    col.cx = scaler::x(80);
    col.fmt = LVCFMT_LEFT;

    ListView_InsertColumn(hPeersList, 1, &col);

    col.pszText = L"Flags";
    col.cx = scaler::x(80);
    col.fmt = LVCFMT_LEFT;

    ListView_InsertColumn(hPeersList, 2, &col);

    col.pszText = L"DL";
    col.cx = scaler::x(80);
    col.fmt = LVCFMT_RIGHT;

    ListView_InsertColumn(hPeersList, 3, &col);

    col.pszText = L"UL";
    col.cx = scaler::x(80);
    col.fmt = LVCFMT_RIGHT;

    ListView_InsertColumn(hPeersList, 4, &col);
}

bool peers_page::on_notify(HWND hDlg, LPNMHDR nmhdr, LRESULT &result)
{
    switch (nmhdr->code)
    {
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);
            
            if (inf->item.iItem == -1
                || peers_.size() == 0)
            {
                break;
            }

            peer_item &item = peers_[inf->item.iItem];

            if (inf->item.mask & LVIF_TEXT)
            {
                switch (inf->item.iSubItem)
                {
                case 0:
                {
                    StringCchPrintf(inf->item.pszText, inf->item.cchTextMax, L"%s:%d", item.ip.c_str(), item.port);
                    break;
                }
                case 1:
                {
                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, item.client.c_str());
                    break;
                }
                case 2:
                {
                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, item.flags.c_str());
                    break;
                }
                case 3:
                case 4:
                {
                    TCHAR result[100];

                    StrFormatByteSize64(
                        inf->item.iSubItem == 3
                            ? item.dl
                            : item.ul,
                        result,
                        ARRAYSIZE(result));

                    StringCchPrintf(inf->item.pszText, inf->item.cchTextMax, TEXT("%s/s"), result);
                    break;
                }
                }
            }

            break;
        }
    }

    return false;
}

