#pragma once

#include "../stdafx.h"

#include <memory>
#include <string>
#include <vector>

namespace libtorrent { class session; class torrent_info; }

namespace Controllers
{
class AddTorrentController
{
public:
    AddTorrentController(const std::shared_ptr<libtorrent::session>& session);
    void Execute();
    void Execute(const std::vector<libtorrent::torrent_info>& torrents);

private:
    std::vector<std::wstring> OpenFiles();

    std::shared_ptr<libtorrent::session> m_session;
};
}
