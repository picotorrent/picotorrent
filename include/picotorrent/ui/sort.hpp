#pragma once

#include <functional>

namespace picotorrent
{
namespace ui
{
    class torrent_list_item;

    class sort
    {
    public:
        typedef std::function<bool(const torrent_list_item&, const torrent_list_item&)> sort_func_t;

        static sort_func_t by_download_rate(bool ascending);
		static sort_func_t by_eta(bool ascending);
        static sort_func_t by_name(bool ascending);
        static sort_func_t by_progress(bool ascending);
        static sort_func_t by_queue_position(bool ascending);
        static sort_func_t by_size(bool ascending);
        static sort_func_t by_upload_rate(bool ascending);
    };
}
}
