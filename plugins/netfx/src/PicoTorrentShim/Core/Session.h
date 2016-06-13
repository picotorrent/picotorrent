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
            void RaiseTorrentAdded(TorrentEventArgs^ args);

        private:
            picotorrent::core::session* sess_;
            SessionCallbackHandler* callbackHandler_;
        };
    }
}
