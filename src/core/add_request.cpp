#include <picotorrent/core/add_request.hpp>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/torrent_file.hpp>

namespace lt = libtorrent;
using namespace picotorrent::common;
using picotorrent::core::add_request;
using picotorrent::core::torrent_file;

add_request::add_request()
    : params_(std::make_unique<lt::add_torrent_params>())
{   
}

add_request::~add_request()
{
}

std::wstring add_request::save_path()
{
    return to_wstring(params_->save_path);
}

std::shared_ptr<picotorrent::core::torrent_file> add_request::torrent_file()
{
    if (!params_->ti)
    {
        return nullptr;
    }

    return std::make_shared<picotorrent::core::torrent_file>(*params_->ti);
}

std::wstring add_request::url()
{
    return to_wstring(params_->url);
}

void add_request::set_save_path(const std::wstring &path)
{
    params_->save_path = to_string(path);
}

void add_request::set_torrent_file(const std::shared_ptr<picotorrent::core::torrent_file> &file)
{
    params_->ti = boost::make_shared<lt::torrent_info>(*file->info_);
}

void add_request::set_url(const std::wstring &url)
{
    params_->url = to_string(url);
}
