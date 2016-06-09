#pragma once
#pragma unmanaged

#include <picotorrent/export.hpp>
#include <picotorrent/plugin.hpp>

namespace picotorrent
{
namespace core
{
    class session;
}
namespace netfx
{
    class managed_bridge;

    class clr_plugin : public plugin
    {
    public:
        clr_plugin(core::session*);
        ~clr_plugin();

        void load();
        void unload();

    private:
        managed_bridge* bridge_;
    };
}
}

extern "C"
{
    DLL_EXPORT picotorrent::plugin* create_picotorrent_plugin(picotorrent::core::session* sess)
    {
        return new picotorrent::netfx::clr_plugin(sess);
    }
}
