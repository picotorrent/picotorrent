#pragma once

#include "stdafx.h"

#include <boost/shared_ptr.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace libtorrent
{
    class alert;
    class session;
    class sha1_hash;
    struct stats_metric;
    class torrent_info;
}

namespace pico
{
    class MainFrame;

    class PicoTorrent
    {
    public:
        PicoTorrent(HINSTANCE hInstance);
        ~PicoTorrent();

        bool Init();

        int Run(int nCmdShow = SW_SHOWDEFAULT);

    private:
        void ReadAlerts();
        void HandleAlert(libtorrent::alert* a);
        void DeleteTorrent(const libtorrent::sha1_hash& hash);
        void SaveTorrent(boost::shared_ptr<const libtorrent::torrent_info> info);

        CMessageLoop loop_;
        HANDLE singleInstance_;

        std::shared_ptr<MainFrame> frame_;
        bool shouldRead_;
        std::thread reader_;
        std::unique_ptr<libtorrent::session> session_;
        std::vector<libtorrent::stats_metric> metrics_;
    };
}
