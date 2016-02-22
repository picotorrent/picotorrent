#pragma once

namespace picotorrent
{
    class plugin
    {
    public:
        virtual void load() = 0;
        virtual void unload() = 0;
    };
}
