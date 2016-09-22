#pragma once

#include <memory>

#include <windows.h>

class IPicoTorrent;

namespace Controllers
{
    class ImportTorrentsController
    {
    public:
        ImportTorrentsController(std::shared_ptr<IPicoTorrent> pico);
        void Execute();

    private:
        std::shared_ptr<IPicoTorrent> m_pico;
    };
}
