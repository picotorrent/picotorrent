#pragma once

namespace libtorrent
{
    struct session_handle;
}

namespace pico
{
    class StateManager
    {
    public:
        explicit StateManager(libtorrent::session_handle& session);
        ~StateManager();

    private:
        void LoadState();
        void LoadTorrents();
        void SaveState();
        void SaveTorrents();

    private:
        libtorrent::session_handle& session_;
    };
}
