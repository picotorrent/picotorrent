#pragma once

#include <windows.h>

#include <map>
#include <memory>

namespace libtorrent
{
    class session;
    class sha1_hash;
    struct torrent_handle;
}

namespace Commands
{
    struct RemoveTorrentsCommand;
}

namespace Controllers
{
    class RemoveTorrentsController
    {
    public:
        RemoveTorrentsController(const std::shared_ptr<libtorrent::session>& session, const std::map<libtorrent::sha1_hash, libtorrent::torrent_handle>& torrents);
        ~RemoveTorrentsController();
        
        void Execute(HWND hWndParent, const Commands::RemoveTorrentsCommand& cmd);

    private:
        std::shared_ptr<libtorrent::session> m_session;
        std::map<libtorrent::sha1_hash, libtorrent::torrent_handle> m_torrents;
    };
}
