#pragma once

namespace picotorrent { namespace core { class torrent_info; } }

namespace PicoTorrent
{
    namespace Core
    {
        ref class TorrentInfo : public ITorrentInfo
        {
        public:
            TorrentInfo(picotorrent::core::torrent_info*);
            ~TorrentInfo();

            virtual property System::String^ InfoHash { System::String^ get(); }

        private:
            picotorrent::core::torrent_info* ti_;
        };
    }
}
