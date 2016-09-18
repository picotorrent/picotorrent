#pragma once

#include <memory>

class IPicoTorrent;

namespace Dialogs
{
    class UpdateAvailableDialog
    {
    public:
        UpdateAvailableDialog(std::shared_ptr<IPicoTorrent> pico);
        void Show();

    private:
        std::shared_ptr<IPicoTorrent> m_pico;
    };
}
