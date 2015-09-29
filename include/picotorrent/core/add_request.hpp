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
    class torrent_file;

    class add_request
    {
        friend class session;

    public:
        explicit add_request();
        ~add_request();

        std::wstring save_path();
        std::shared_ptr<torrent_file> torrent_file();
        void set_save_path(const std::wstring &path);
        void set_torrent_file(const std::shared_ptr<core::torrent_file> &file);

    private:
        std::unique_ptr<libtorrent::add_torrent_params> params_;
    };
}
}
