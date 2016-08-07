#pragma once

namespace PicoTorrent
{
    namespace Core
    {
        ref class TorrentInfoParser : public ITorrentInfoParser
        {
        public:
            virtual ITorrentInfo^ Parse(System::String^ fileName);
        };
    }
}
