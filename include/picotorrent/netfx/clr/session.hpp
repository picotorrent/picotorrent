#pragma once

namespace picotorrent { namespace core { class session; } }

namespace PicoTorrent
{
namespace Core
{
    ref class Session
    {
    public:
        Session(picotorrent::core::session *s);

    private:
        picotorrent::core::session* s_;
    };
}
}
