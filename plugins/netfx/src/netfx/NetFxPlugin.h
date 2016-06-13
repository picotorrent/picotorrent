#pragma once
#pragma unmanaged

#include <picotorrent/plugin.hpp>
#include <windows.h>

namespace picotorrent { namespace core { class session; } }

class ClrBridge;

class NetFxPlugin : public picotorrent::plugin
{
public:
    NetFxPlugin(picotorrent::core::session*, HWND hWnd);
    ~NetFxPlugin();

    void load();
    void unload();

private:
    ClrBridge* bridge_;
};

extern "C"
{
    __declspec(dllexport) picotorrent::plugin* create_picotorrent_plugin(picotorrent::core::session* sess, HWND hWnd)
    {
        return new NetFxPlugin(sess, hWnd);
    }
}
