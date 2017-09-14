#pragma once

#include "../stdafx.h"

#include <libtorrent/sha1_hash.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace libtorrent { struct add_torrent_params; class session; class torrent_info; }

namespace Controllers
{
class AddTorrentController
{
public:
    explicit AddTorrentController(HWND hWndOwner, const std::shared_ptr<libtorrent::session>& session);

    void Execute();
	void Execute(std::vector<libtorrent::add_torrent_params>& params);
    void Execute(const std::vector<std::wstring>& files);
    void ExecuteMagnets(const std::vector<std::wstring>& magnetLinks);

private:
    std::vector<std::wstring> OpenFiles();

    HWND m_hWndOwner;
    std::shared_ptr<libtorrent::session> m_session;
};
}
