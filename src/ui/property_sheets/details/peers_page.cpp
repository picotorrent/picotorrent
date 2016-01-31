#include <picotorrent/ui/property_sheets/details/peers_page.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/peer.hpp>
#include <picotorrent/ui/controls/list_view.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <cassert>
#include <shlwapi.h>
#include <strsafe.h>

#define LIST_COLUMN_IP 1
#define LIST_COLUMN_CLIENT 2
#define LIST_COLUMN_FLAGS 3
#define LIST_COLUMN_DOWNLOAD 4
#define LIST_COLUMN_UPLOAD 5

using picotorrent::common::to_wstring;
using picotorrent::core::peer;
using picotorrent::ui::controls::list_view;
using picotorrent::ui::property_sheets::details::peers_page;
using picotorrent::ui::scaler;

struct peers_page::peer_state
{
    peer_state(const peer &p)
        : peer(p)
    {
    }

    peer peer;
    bool dirty;
};

peers_page::peers_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_PEERS);
    set_title_id(IDS_DETAILS_PEERS_TITLE);
}

peers_page::~peers_page()
{
    std::unique_lock<std::mutex> lock(update_mtx_);
}

void peers_page::refresh(const std::vector<peer> &peers)
{
    std::unique_lock<std::mutex> lock(update_mtx_);

    for (peer_state &ps : peers_)
    {
        ps.dirty = false;
    }

    for (const peer &p : peers)
    {
        // TODO: std::find_if uses moderate CPU here, and in a loop as well. maybe a std::map is better for peers_.
        auto &item = std::find_if(peers_.begin(), peers_.end(), [p](const peer_state &ps) { return ps.peer.id() == p.id(); });

        if (item != peers_.end())
        {
            item->peer = p;
            item->dirty = true;
        }
        else
        {
            peer_state state(p);
            state.dirty = true;
            peers_.push_back(state);
        }
    }

    peers_.erase(std::remove_if(peers_.begin(), peers_.end(),
        [](const peer_state &p)
    {
        return !p.dirty;
    }), peers_.end());

    assert(peers_.size() == peers.size());
    list_->set_item_count((int)peers_.size());
}

BOOL peers_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void peers_page::on_init_dialog()
{
    HWND hList = GetDlgItem(handle(), ID_DETAILS_PEERS_LIST);
    list_ = std::make_unique<list_view>(hList);

    list_->add_column(LIST_COLUMN_IP,       L"IP",     scaler::x(110));
    list_->add_column(LIST_COLUMN_CLIENT,   L"Client", scaler::x(140));
    list_->add_column(LIST_COLUMN_FLAGS,    L"Flags",  scaler::x(80));
    list_->add_column(LIST_COLUMN_DOWNLOAD, L"DL",     scaler::x(80), list_view::number);
    list_->add_column(LIST_COLUMN_UPLOAD,   L"UL",     scaler::x(80), list_view::number);

    list_->on_display().connect(std::bind(&peers_page::on_list_display, this, std::placeholders::_1));
}

std::wstring peers_page::on_list_display(const std::pair<int, int> &p)
{
    std::unique_lock<std::mutex> lock(update_mtx_);

    peer_state &ps = peers_[p.second];

    switch (p.first)
    {
    case LIST_COLUMN_IP:
        TCHAR res[100];
        StringCchPrintf(res, ARRAYSIZE(res), L"%s:%d", to_wstring(ps.peer.ip()).c_str(), ps.peer.port());
        return res;
    case LIST_COLUMN_CLIENT:
        return to_wstring(ps.peer.client());
    case LIST_COLUMN_FLAGS:
        return to_wstring(ps.peer.flags_str());
    case LIST_COLUMN_DOWNLOAD:
    case LIST_COLUMN_UPLOAD:
    {
        TCHAR result[100];

        StrFormatByteSize64(
            p.first == LIST_COLUMN_DOWNLOAD
            ? ps.peer.download_rate()
            : ps.peer.upload_rate(),
            result,
            ARRAYSIZE(result));

        StringCchPrintf(result, ARRAYSIZE(result), TEXT("%s/s"), result);
        return result;
    }
    default:
        return L"<unknown column>";
    }
}
