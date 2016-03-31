#pragma once

#include <picotorrent/core/signals/signal.hpp>

#include <string>
#include <vector>
#include <windows.h>

namespace picotorrent
{
namespace client
{
namespace ui
{
    class torrent_drop_target
    {
    public:
        torrent_drop_target(HWND hParent);
        ~torrent_drop_target();

        core::signals::signal_connector<void, const std::vector<std::string>&>& on_torrents_dropped();

    private:
        HWND parent_;
        long ref_;
        core::signals::signal<void, const std::vector<std::string>&> on_torrents_dropped_;
    };
}
}
}
