#pragma once

#include <libtorrent/sha1_hash.hpp>
#include <picotorrent/api.hpp>

#include <map>
#include <thread>
#include <vector>

namespace libtorrent
{
    struct torrent_status;
}

namespace API
{
    class PicoTorrent : public IPicoTorrent
    {
    public:
        PicoTorrent(HWND hWndOwner, std::shared_ptr<libtorrent::session> session);

        void EmitTorrentAdded(libtorrent::torrent_status& status);
        void EmitTorrentRemoved(libtorrent::sha1_hash const& infoHash);

        void AddMenuItem(MenuItem const& item);
        std::shared_ptr<picojson::object> GetConfiguration();
        std::string GetCurrentVersion();
        std::shared_ptr<IFileSystem> GetFileSystem();
        std::shared_ptr<ILogger> GetLogger();
        std::shared_ptr<libtorrent::session> GetSession();
        std::shared_ptr<ITranslator> GetTranslator();
        void RegisterEventSink(std::shared_ptr<ITorrentEventSink> sink);
        std::unique_ptr<TaskDialogResult> ShowTaskDialog(TASKDIALOGCONFIG* tdcfg);

    private:
        static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

        std::vector<std::shared_ptr<ITorrentEventSink>> m_torrentSinks;

        HWND m_hWndOwner;
        std::shared_ptr<libtorrent::session> m_session;
        std::map<int, MenuItem> m_menuItems;
        std::thread::id m_threadId;
    };
}
