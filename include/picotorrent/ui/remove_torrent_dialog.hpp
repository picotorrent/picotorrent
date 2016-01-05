#pragma once

#include <windows.h>

#define ID_REMOVE_DATA 100
#define ID_KEEP_DATA 200

namespace picotorrent
{
namespace ui
{
    class remove_torrent_dialog
    {
    public:
        remove_torrent_dialog();
        ~remove_torrent_dialog();

        int show(HWND parent, bool remove_files);
    };
}
}
