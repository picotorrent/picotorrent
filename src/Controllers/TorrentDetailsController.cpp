#include "TorrentDetailsController.hpp"

#include <libtorrent/torrent_handle.hpp>

#include "../PropertySheets/Details/DetailsSheet.hpp"

namespace lt = libtorrent;
using Controllers::TorrentDetailsController;

TorrentDetailsController::TorrentDetailsController(HWND hWndParent, const lt::torrent_handle& torrent)
    : m_hWndParent(hWndParent),
    m_torrent(torrent)
{
}

void TorrentDetailsController::Execute()
{
    PropertySheets::Details::DetailsSheet details(m_torrent);
    details.DoModal(m_hWndParent);
}
