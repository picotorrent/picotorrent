#pragma once

#include <memory>
#include <string>

namespace libtorrent
{
    struct add_torrent_params;
}

namespace picotorrent
{
namespace core
{
    class session;
    class torrent_info;

    class add_request
    {
        friend class session;

    public:
        explicit add_request();
        ~add_request();

        int file_priority(int file_index);
        std::wstring save_path();
        std::shared_ptr<torrent_info> torrent_info();
        std::wstring url();
        void set_file_priority(int file_index, int priority);
        void set_save_path(const std::wstring &path);
        void set_torrent_info(const std::shared_ptr<core::torrent_info> &file);
        void set_url(const std::wstring &url);

    private:
        std::unique_ptr<libtorrent::add_torrent_params> params_;
    };
}
}
