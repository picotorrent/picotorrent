#include <picotorrent/ui/torrent_list_item.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/torrent.hpp>
#include <windows.h>
#include <shlwapi.h>

namespace core = picotorrent::core;
using namespace picotorrent::common;
using picotorrent::ui::torrent_list_item;

torrent_list_item::torrent_list_item(const std::shared_ptr<core::torrent> &torrent)
    : torrent_(torrent)
{
}

int torrent_list_item::download_rate() const
{
    return torrent_->download_rate();
}

std::wstring torrent_list_item::download_rate_str() const
{
    return get_speed(torrent_->download_rate());
}

std::wstring torrent_list_item::name() const
{
    return to_wstring(torrent_->name());
}

float torrent_list_item::progress() const
{
    return torrent_->progress();
}

int torrent_list_item::queue_position() const
{
    return torrent_->queue_position();
}

std::wstring torrent_list_item::queue_position_str() const
{
    if (torrent_->queue_position() >= 0)
    {
        return std::to_wstring(torrent_->queue_position() + 1);
    }

    return L"-";
}

int64_t torrent_list_item::size() const
{
    return torrent_->size();
}

std::wstring torrent_list_item::size_str() const
{
    int64_t size = torrent_->size();

    if (size < 0)
    {
        return L"<unknown>";
    }

    std::wstring result;
    result.resize(128);

    StrFormatByteSize64(
        size,
        &result[0],
        result.size());

    return result;
}

int torrent_list_item::upload_rate() const
{
    return torrent_->upload_rate();
}

std::wstring torrent_list_item::upload_rate_str() const
{
    return get_speed(torrent_->upload_rate());
}

std::wstring torrent_list_item::get_speed(int rate) const
{
    if (rate < 1024)
    {
        return L"-";
    }

    std::wstring result;
    result.resize(128);

    StrFormatByteSize64(
        rate,
        &result[0],
        result.size());

    return result + L"/s";
}
