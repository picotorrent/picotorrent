#pragma once

#include <windows.h>

#include <map>
#include <memory>
#include <vector>

#include <libtorrent/sha1_hash.hpp>

namespace libtorrent
{
    class session;
    struct torrent_handle;
}

struct Torrent;

namespace Controllers
{
    class RemoveTorrentsController
    {
    public:
        RemoveTorrentsController(HWND hWndParent, const std::shared_ptr<libtorrent::session>& session, const std::map<libtorrent::sha1_hash, libtorrent::torrent_handle>& torrents);
        ~RemoveTorrentsController();
        
        void Execute(const std::vector<Torrent>& torrents, bool removeData = false);

    private:
        HWND m_hWndParent;
        std::shared_ptr<libtorrent::session> m_session;
        std::map<libtorrent::sha1_hash, libtorrent::torrent_handle> m_torrents;
    };
}
