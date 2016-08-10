#pragma once

#include <memory>
#include <picotorrent/export.hpp>
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
        enum allocation_mode_t
        {
            sparse,
            full
        };

        DLL_EXPORT explicit add_request();
        DLL_EXPORT ~add_request();

        DLL_EXPORT int file_priority(int file_index);
        DLL_EXPORT std::string name();
        DLL_EXPORT std::string save_path();
        DLL_EXPORT std::shared_ptr<torrent_info> ti();
        DLL_EXPORT std::string url();
        DLL_EXPORT void set_allocation_mode(allocation_mode_t mode);
        DLL_EXPORT void set_file_priority(int file_index, int priority);
        DLL_EXPORT void set_save_path(const std::string &path);
        DLL_EXPORT void set_torrent_info(const core::torrent_info &file);
        DLL_EXPORT void set_url(const std::string &url);

    private:
        std::unique_ptr<libtorrent::add_torrent_params> params_;
    };
}
}
