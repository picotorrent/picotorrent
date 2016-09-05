#pragma once

#include <windows.h>

#include <memory>
#include <vector>

namespace libtorrent
{
    class session;
    class sha1_hash;
}

class SessionManager
{
public:
    SessionManager(HWND hWndOwner);
    ~SessionManager();

    void LoadState();
    void LoadTorrents();
    void SaveState();
    void SaveTorrents();

private:
    HWND m_hWndOwner;
    std::shared_ptr<libtorrent::session> m_session;
    std::vector<libtorrent::sha1_hash> m_muted_hashes;
};
