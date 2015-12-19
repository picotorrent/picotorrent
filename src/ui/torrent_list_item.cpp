#include <picotorrent/ui/torrent_list_item.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/torrent.hpp>
#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>

namespace core = picotorrent::core;
using namespace picotorrent::common;
using picotorrent::ui::torrent_list_item;

bool picotorrent::ui::operator==(const torrent_list_item &i1, const torrent_list_item &i2)
{
    return i1.torrent_ == i2.torrent_;
}

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

int torrent_list_item::eta() const
{
	return torrent_->eta();
}

std::wstring torrent_list_item::eta_str() const
{
	int seconds = torrent_->eta();

	if (seconds < 0)
	{
		return L"-";
	}

	if(seconds == 0)
	{
		return L"0";
	}

	if (seconds < 60)
	{
		return L"< 1m";
	}

	int minutes = seconds / 60;

	if (minutes < 60)
	{
		return std::to_wstring(minutes) + L"m";
	}

	int hours = minutes / 60;
	minutes = minutes - hours * 60;

	if (hours < 24)
	{
		return std::to_wstring(hours) + L"h " + std::to_wstring(minutes) + L"m";
	}

	int days = hours / 24;
	hours = hours - days * 24;

	if (days < 100)
	{
		return std::to_wstring(days) + L"d " + std::to_wstring(hours) + L"h";
	}

	return L"-";
}

std::wstring torrent_list_item::name() const
{
    return to_wstring(torrent_->name());
}

float torrent_list_item::progress() const
{
    return torrent_->progress();
}

std::wstring torrent_list_item::progress_str() const
{
    TCHAR p[100];
    StringCchPrintf(p, ARRAYSIZE(p), TEXT("%.2f%%"), torrent_->progress() * 100);
    return p;
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
        (DWORD)result.size());

    return result;
}

std::wstring torrent_list_item::state_str() const
{
    // TODO: translations
    return torrent_->state().to_string();
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

    TCHAR result[100];

    StrFormatByteSize64(
        rate,
        result,
        ARRAYSIZE(result));

    StringCchPrintf(result, ARRAYSIZE(result), TEXT("%s/s"), result);

    return result;
}

std::shared_ptr<core::torrent> torrent_list_item::torrent() const
{
    return torrent_;
}
