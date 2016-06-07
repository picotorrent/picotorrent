#include <picotorrent/client/ui/property_sheets/details/peers_page.hpp>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/controls/list_view.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/scaler.hpp>
#include <picotorrent/core/peer.hpp>
#include <picotorrent/common/string_operations.hpp>

#include <cassert>
#include <shlwapi.h>
#include <strsafe.h>

#define LIST_COLUMN_IP 1
#define LIST_COLUMN_CLIENT 2
#define LIST_COLUMN_FLAGS 3
#define LIST_COLUMN_DOWNLOAD 4
#define LIST_COLUMN_UPLOAD 5

using picotorrent::client::ui::controls::list_view;
using picotorrent::client::ui::property_sheets::details::peers_page;
using picotorrent::client::ui::scaler;
using picotorrent::common::to_string;
using picotorrent::common::to_wstring;
using picotorrent::core::peer;

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
    set_title(TR("peers"));
}

peers_page::~peers_page()
{
}

void peers_page::refresh(const std::vector<peer> &peers)
{
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

    if (sort_items_)
    {
        sort_items_();
    }
}

BOOL peers_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void peers_page::on_init_dialog()
{
    HWND hList = GetDlgItem(handle(), ID_DETAILS_PEERS_LIST);
    list_ = std::make_unique<list_view>(hList);

    list_->add_column(LIST_COLUMN_IP,       TR("ip"),     scaler::x(110));
    list_->add_column(LIST_COLUMN_CLIENT,   TR("client"), scaler::x(140));
    list_->add_column(LIST_COLUMN_FLAGS,    TR("flags"),  scaler::x(80));
    list_->add_column(LIST_COLUMN_DOWNLOAD, TR("dl"),     scaler::x(80), list_view::number);
    list_->add_column(LIST_COLUMN_UPLOAD,   TR("ul"),     scaler::x(80), list_view::number);

    list_->load_state("torrent_peers_list");
    on_destroy().connect([this]()
    {
        list_->save_state("torrent_peers_list");
    });

    list_->on_display().connect(std::bind(&peers_page::on_list_display, this, std::placeholders::_1));
    list_->on_sort().connect(std::bind(&peers_page::on_list_sort, this, std::placeholders::_1));
}

std::string peers_page::on_list_display(const std::pair<int, int> &p)
{
    peer_state &ps = peers_[p.second];

    switch (p.first)
    {
    case LIST_COLUMN_IP:
        TCHAR res[100];
        StringCchPrintf(res, ARRAYSIZE(res), L"%s:%d", to_wstring(ps.peer.ip()).c_str(), ps.peer.port());
        return to_string(res);
    case LIST_COLUMN_CLIENT:
        return ps.peer.client();
    case LIST_COLUMN_FLAGS:
        return ps.peer.flags_str();
    case LIST_COLUMN_DOWNLOAD:
    case LIST_COLUMN_UPLOAD:
    {
        int rate = p.first == LIST_COLUMN_DOWNLOAD ? ps.peer.download_rate() : ps.peer.upload_rate();

        if (rate < 1024)
        {
            return "-";
        }

        TCHAR result[100];
        StrFormatByteSize64(rate, result, ARRAYSIZE(result));
        StringCchPrintf(result, ARRAYSIZE(result), L"%s/s", result);
        return to_string(result);
    }
    default:
        return "<unknown column>";
    }
}

void peers_page::on_list_sort(const std::pair<int, int> &p)
{
    list_view::sort_order_t order = (list_view::sort_order_t)p.second;
    bool asc = order == list_view::sort_order_t::asc;

    std::function<bool(const peer_state&, const peer_state&)> sort_func;

    switch (p.first)
    {
    case LIST_COLUMN_IP:
        sort_func = [asc](const peer_state &p1, const peer_state &p2)
        {
            if (asc) { return p1.peer.ip() < p2.peer.ip(); }
            return p1.peer.ip() > p2.peer.ip();
        };
        break;
    case LIST_COLUMN_CLIENT:
        sort_func = [asc](const peer_state &p1, const peer_state &p2)
        {
            if (asc) { return p1.peer.client() < p2.peer.client(); }
            return p1.peer.client() > p2.peer.client();
        };
        break;
    case LIST_COLUMN_FLAGS:
        sort_func = [asc](const peer_state &p1, const peer_state &p2)
        {
            if (asc) { return p1.peer.flags_str() < p2.peer.flags_str(); }
            return p1.peer.flags_str() > p2.peer.flags_str();
        };
        break;
    case LIST_COLUMN_DOWNLOAD:
        sort_func = [asc](const peer_state &p1, const peer_state &p2)
        {
            if (asc) { return p1.peer.download_rate() < p2.peer.download_rate(); }
            return p1.peer.download_rate() > p2.peer.download_rate();
        };
        break;
    case LIST_COLUMN_UPLOAD:
        sort_func = [asc](const peer_state &p1, const peer_state &p2)
        {
            if (asc) { return p1.peer.upload_rate() < p2.peer.upload_rate(); }
            return p1.peer.upload_rate() > p2.peer.upload_rate();
        };
        break;
    }

    if (sort_func)
    {
        sort_items_ = [this, sort_func]()
        {
            std::sort(peers_.begin(), peers_.end(), sort_func);
        };
        sort_items_();
    }
}
