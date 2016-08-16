#pragma once

#include "../stdafx.h"

#include <memory>
#include <string>
#include <vector>

namespace libtorrent { class session; }

namespace Controllers
{
class AddTorrentController
{
public:
    AddTorrentController(const std::shared_ptr<libtorrent::session>& session);
    void Execute();

private:
    std::vector<std::wstring> OpenFiles();

    std::shared_ptr<libtorrent::session> m_session;
};
}
