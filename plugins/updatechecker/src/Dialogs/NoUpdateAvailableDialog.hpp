#pragma once

#include <memory>

class IPicoTorrent;

namespace Dialogs
{
    class NoUpdateAvailableDialog
    {
    public:
        NoUpdateAvailableDialog(std::shared_ptr<IPicoTorrent> pico);
        void Show();

    private:
        std::shared_ptr<IPicoTorrent> m_pico;
    };
}
