#pragma once

namespace picotorrent { namespace core { class session; } }
struct SessionCallbackHandler;

namespace PicoTorrent
{
    namespace Core
    {
        ref class Session : public ISession
        {
        public:
            Session(picotorrent::core::session*);

            virtual event System::EventHandler<TorrentEventArgs^>^ TorrentAdded;
            virtual event System::EventHandler<TorrentEventArgs^>^ TorrentFinished;
            virtual event System::EventHandler<TorrentEventArgs^>^ TorrentRemoved;

            virtual void AddTorrent(System::String^ torrentFile, System::String^ savePath);

            void RaiseTorrentAdded(TorrentEventArgs^ args);
            void RaiseTorrentFinished(TorrentEventArgs^ args);
            void RaiseTorrentRemoved(TorrentEventArgs^ args);

        private:
            picotorrent::core::session* sess_;
            SessionCallbackHandler* callbackHandler_;
        };
    }
}
