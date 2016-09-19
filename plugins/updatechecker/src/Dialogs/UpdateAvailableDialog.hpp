#pragma once

#include <memory>
#include <string>

class IPicoTorrent;

namespace Dialogs
{
    class UpdateAvailableDialog
    {
    public:
        UpdateAvailableDialog(std::shared_ptr<IPicoTorrent> pico);
        void Show(std::wstring const& version, std::wstring const& url);

    private:
        std::shared_ptr<IPicoTorrent> m_pico;
    };
}
