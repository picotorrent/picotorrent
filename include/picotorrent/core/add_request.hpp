#pragma once

#include <memory>
#include <picotorrent/common.hpp>
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
        DLL_EXPORT explicit add_request();
        DLL_EXPORT ~add_request();

        DLL_EXPORT int file_priority(int file_index);
        DLL_EXPORT std::wstring name();
        DLL_EXPORT std::wstring save_path();
        DLL_EXPORT std::shared_ptr<torrent_info> torrent_info();
        DLL_EXPORT std::wstring url();
        DLL_EXPORT void set_file_priority(int file_index, int priority);
        DLL_EXPORT void set_save_path(const std::wstring &path);
        DLL_EXPORT void set_torrent_info(const std::shared_ptr<core::torrent_info> &file);
        DLL_EXPORT void set_url(const std::wstring &url);

    private:
        std::unique_ptr<libtorrent::add_torrent_params> params_;
    };
}
}