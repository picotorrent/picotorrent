#include <picotorrent/core/add_request.hpp>

#include <picotorrent/core/torrent_info.hpp>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

namespace lt = libtorrent;
using picotorrent::core::add_request;
using picotorrent::core::torrent_info;

#define FILE_PRIO_NORMAL 4

add_request::add_request()
    : params_(new lt::add_torrent_params())
{
    params_->file_priorities = std::vector<uint8_t>();
}

add_request::~add_request()
{
}

int add_request::file_priority(int file_index)
{
    if (params_->file_priorities.size() < (size_t)(file_index + 1))
    {
        return FILE_PRIO_NORMAL; // Normal priority according to libtorrent
    }

    return params_->file_priorities[file_index];
}

std::string add_request::name()
{
    return params_->name;
}

std::string add_request::save_path()
{
    return params_->save_path;
}

std::shared_ptr<picotorrent::core::torrent_info> add_request::ti()
{
    if (!params_->ti)
    {
        return nullptr;
    }

    return std::make_shared<picotorrent::core::torrent_info>(*params_->ti);
}

std::string add_request::url()
{
    return params_->url;
}

void add_request::set_allocation_mode(add_request::allocation_mode_t mode)
{
    if (mode == add_request::allocation_mode_t::sparse)
    {
        params_->storage_mode = lt::storage_mode_t::storage_mode_sparse;
    }
    else
    {
        params_->storage_mode = lt::storage_mode_t::storage_mode_allocate;
    }
}

void add_request::set_file_priority(int file_index, int priority)
{
    if (params_->file_priorities.size() < (size_t)(file_index + 1))
    {
        params_->file_priorities.resize(file_index + 1, FILE_PRIO_NORMAL);
    }

    params_->file_priorities[file_index] = priority;
}

void add_request::set_save_path(const std::string &path)
{
    params_->save_path = path;
}

void add_request::set_torrent_info(const std::shared_ptr<picotorrent::core::torrent_info> &file)
{
    params_->ti = boost::make_shared<lt::torrent_info>(*file->info_);
}

void add_request::set_url(const std::string &url)
{
    if (url.substr(0, 10) == "magnet:?xt")
    {
        lt::error_code ec;
        lt::parse_magnet_uri(url, *params_, ec);
    }
    else
    {
        params_->url = url;
    }
}
