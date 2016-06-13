#pragma once

namespace picotorrent { namespace core { class torrent; } }

namespace PicoTorrent
{
namespace Core
{
    ref class Torrent : public ITorrent
    {
    public:
        Torrent(picotorrent::core::torrent*);

        virtual property System::String^ Name { System::String^ get(); }

        virtual void Pause();

        virtual void Resume(bool force);

    private:
        picotorrent::core::torrent* torrent_;
    };

}
}
