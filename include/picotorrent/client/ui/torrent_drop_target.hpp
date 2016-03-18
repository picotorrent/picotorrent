#pragma once

#include <picotorrent/core/signals/signal.hpp>

#include <vector>
#include <windows.h>

namespace picotorrent
{
namespace core
{
namespace filesystem
{
    class path;
}
}
namespace client
{
namespace ui
{
    class torrent_drop_target
    {
    public:
        torrent_drop_target(HWND hParent);
        ~torrent_drop_target();

        core::signals::signal_connector<void, const std::vector<core::filesystem::path>&>& on_torrents_dropped();

    private:
        HWND parent_;
        long ref_;
        core::signals::signal<void, const std::vector<core::filesystem::path>&> on_torrents_dropped_;
    };
}
}
}
