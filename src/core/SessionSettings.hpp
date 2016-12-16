#pragma once

namespace libtorrent
{
    struct settings_pack;
}

namespace Core
{
    struct SessionSettings
    {
        static libtorrent::settings_pack Get();
    };
}
