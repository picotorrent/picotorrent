#include <picotorrent/ui/sort.hpp>

#include <picotorrent/ui/torrent_list_item.hpp>

using picotorrent::ui::sort;
using picotorrent::ui::torrent_list_item;

sort::sort_func_t sort::by_download_rate(bool ascending)
{
    return [ascending](const torrent_list_item &lhs, const torrent_list_item &rhs)
    {
        if (ascending)
        {
            return lhs.download_rate() < rhs.download_rate();
        }

        return lhs.download_rate() > rhs.download_rate();
    };
}

sort::sort_func_t sort::by_name(bool ascending)
{
    return [ascending](const torrent_list_item &lhs, const torrent_list_item &rhs)
    {
        if (ascending)
        {
            return lhs.name() < rhs.name();
        }

        return lhs.name() > rhs.name();
    };
}

sort::sort_func_t sort::by_progress(bool ascending)
{
    return [ascending](const torrent_list_item &lhs, const torrent_list_item &rhs)
    {
        if (ascending)
        {
            return lhs.progress() < rhs.progress();
        }

        return lhs.size() > rhs.size();
    };
}

sort::sort_func_t sort::by_queue_position(bool ascending)
{
    return [ascending](const torrent_list_item &lhs, const torrent_list_item &rhs)
    {
        // TODO: queue position -1 should always come last

        if (ascending)
        {
            return lhs.queue_position() < rhs.queue_position();
        }

        return lhs.queue_position() > rhs.queue_position();
    };
}

sort::sort_func_t sort::by_size(bool ascending)
{
    return [ascending](const torrent_list_item &lhs, const torrent_list_item &rhs)
    {
        if (ascending)
        {
            return lhs.size() < rhs.size();
        }

        return lhs.size() > rhs.size();
    };
}

sort::sort_func_t sort::by_upload_rate(bool ascending)
{
    return [ascending](const torrent_list_item &lhs, const torrent_list_item &rhs)
    {
        if (ascending)
        {
            return lhs.upload_rate() < rhs.upload_rate();
        }

        return lhs.upload_rate() > rhs.upload_rate();
    };
}
