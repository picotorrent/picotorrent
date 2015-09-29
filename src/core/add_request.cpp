#include <picotorrent/core/add_request.hpp>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <picotorrent/core/torrent_file.hpp>

namespace lt = libtorrent;
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
    return L"";
}

std::shared_ptr<picotorrent::core::torrent_file> add_request::torrent_file()
{
    if (!params_->ti)
    {
        return nullptr;
    }

    return std::make_shared<picotorrent::core::torrent_file>(*params_->ti);
}

void add_request::set_save_path(const std::wstring &path)
{
    // TODO: move conversion to util function
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &path[0], (int)path.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &path[0], (int)path.size(), &strTo[0], size_needed, NULL, NULL);
    
    params_->save_path = strTo;
}

void add_request::set_torrent_file(const std::shared_ptr<picotorrent::core::torrent_file> &file)
{
    params_->ti = boost::make_shared<lt::torrent_info>(*file->info_);
}
