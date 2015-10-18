#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "stdafx.h"

namespace libtorrent
{
    class session;
    class sha1_hash;
    struct torrent_handle;
}

namespace pico
{
    class TorrentItem;
    class TorrentListView;

    class MainWindow
    {
    public:
        MainWindow();
        ~MainWindow();

        void Create();

    private:
        typedef HANDLE HTHEME;

        void AddTorrent(std::wstring const& file);
        void LoadState();
        void LoadTorrents();
        void OnAlertNotify(HWND target);
        void OnDestroy();
        void OnFileAddTorrent();
        LRESULT CALLBACK WndProc(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
        static LRESULT CALLBACK WndProcProxy(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);

        HWND hWnd_;
        HWND hWnd_progress_;
        HTHEME hTheme_progress_;
        UINT_PTR timer_;

        std::map<libtorrent::sha1_hash, TorrentItem> torrents_;
        std::map<int, libtorrent::sha1_hash> items_;
        std::unique_ptr<libtorrent::session> session_;
        std::unique_ptr<TorrentListView> torrentsView_;
    };
}
