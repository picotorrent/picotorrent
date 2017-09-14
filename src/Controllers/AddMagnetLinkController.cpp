#include "AddMagnetLinkController.hpp"

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include "AddTorrentController.hpp"
#include "../Dialogs/AddMagnetLinkDialog.hpp"

namespace lt = libtorrent;
using Controllers::AddMagnetLinkController;

AddMagnetLinkController::AddMagnetLinkController(HWND hWndOwner, const std::shared_ptr<lt::session>& session)
    : m_hWndOwner(hWndOwner),
    m_session(session)
{
}

void AddMagnetLinkController::Execute()
{
    Execute(std::vector<std::wstring>());
}

void AddMagnetLinkController::Execute(const std::vector<std::wstring>& magnetLinks)
{
    Dialogs::AddMagnetLinkDialog dlg(magnetLinks);

    if (dlg.DoModal(m_hWndOwner) == IDOK)
    {
        std::vector<lt::add_torrent_params> params = dlg.GetTorrentParams();

        AddTorrentController atc(m_hWndOwner, m_session);
        atc.Execute(params);
    }
}

