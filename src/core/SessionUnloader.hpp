#pragma once

#include <memory>

namespace libtorrent
{
    class session;
}

namespace Core
{
    struct SessionUnloader
    {
        static void Unload(const std::shared_ptr<libtorrent::session>& session);
    };
}
