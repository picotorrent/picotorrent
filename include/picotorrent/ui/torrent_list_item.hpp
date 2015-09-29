#pragma once

#include <memory>
#include <string>
#include <windows.h>

namespace picotorrent
{
namespace core
{
    class torrent;
}
namespace ui
{
    class torrent_list_item
    {
    public:
        torrent_list_item(const std::shared_ptr<core::torrent> &torrent);

        int download_rate() const;
        std::wstring download_rate_str() const;
        std::wstring name() const;
        float progress() const;
        int queue_position() const;
        std::wstring queue_position_str() const;
        int64_t size() const;
        std::wstring size_str() const;
        int upload_rate() const;
        std::wstring upload_rate_str() const;

    private:
        std::wstring get_speed(int rate) const;
        std::shared_ptr<core::torrent> torrent_;
    };
}
}
