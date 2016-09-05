#include "OptionsPage.hpp"

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include <strsafe.h>

#include "../../resources.h"
#include "../../Configuration.hpp"
#include "../../Translator.hpp"

namespace lt = libtorrent;
using PropertySheets::Details::OptionsPage;

OptionsPage::OptionsPage(const lt::torrent_handle& torrent)
    : m_torrent(torrent)
{
    m_title = TRW("options");
    SetTitle(m_title.c_str());
}

BOOL OptionsPage::OnApply()
{
    std::wstring dlLimitText = m_dlLimit.GetValueW();
    std::wstring ulLimitText = m_ulLimit.GetValueW();
    std::wstring maxConnectionsText = m_maxConnections.GetValueW();
    std::wstring maxUploadsText = m_maxUploads.GetValueW();

    int dlLimit = dlLimitText.empty() ? -1 : std::stoi(dlLimitText);
    int ulLimit = ulLimitText.empty() ? -1 : std::stoi(ulLimitText);
    int maxConnections = maxConnectionsText.empty() ? -1 : std::stoi(maxConnectionsText);
    int maxUploads = maxUploadsText.empty() ? -1 : std::stoi(maxUploadsText);
    bool sequentialDownload = m_sequentialDownload.IsChecked();

    if (dlLimit > 0) { dlLimit *= 1024; }
    if (ulLimit > 0) { ulLimit *= 1024; }

    m_torrent.set_download_limit(dlLimit);
    m_torrent.set_max_connections(maxConnections);
    m_torrent.set_max_uploads(maxUploads);
    m_torrent.set_sequential_download(sequentialDownload);
    m_torrent.set_upload_limit(ulLimit);

    return TRUE;
}

void OptionsPage::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    switch (nID)
    {
    case ID_DL_LIMIT:
    case ID_UL_LIMIT:
    case ID_MAX_CONNECTIONS:
    case ID_MAX_UPLOADS:
        if (uNotifyCode == EN_CHANGE) { SetModified(); }
        break;
    case ID_SEQUENTIAL_DOWNLOAD:
        m_sequentialDownload.Toggle();
        SetModified();
        break;
    }
}

BOOL OptionsPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    // Set up UI
    m_dlLimit = GetDlgItem(ID_DL_LIMIT);
    m_ulLimit = GetDlgItem(ID_UL_LIMIT);
    m_maxConnections = GetDlgItem(ID_MAX_CONNECTIONS);
    m_maxUploads = GetDlgItem(ID_MAX_UPLOADS);
    m_sequentialDownload = GetDlgItem(ID_SEQUENTIAL_DOWNLOAD);

    // Translate
    SetDlgItemText(ID_LIMITS_GROUP, TRW("limits"));
    SetDlgItemText(ID_DL_LIMIT_TEXT, TRW("dl_limit"));
    SetDlgItemText(ID_DL_LIMIT_HELP, TRW("dl_limit_help"));
    SetDlgItemText(ID_UL_LIMIT_TEXT, TRW("ul_limit"));
    SetDlgItemText(ID_UL_LIMIT_HELP, TRW("ul_limit_help"));
    SetDlgItemText(ID_MAX_CONNECTIONS_TEXT, TRW("max_connections"));
    SetDlgItemText(ID_MAX_CONNECTIONS_HELP, TRW("max_connections_help"));
    SetDlgItemText(ID_MAX_UPLOADS_TEXT, TRW("max_uploads"));
    SetDlgItemText(ID_MAX_UPLOADS_HELP, TRW("max_uploads_help"));
    SetDlgItemText(ID_SEQUENTIAL_DOWNLOAD, TRW("sequential_download"));

    // Show data on controls
    lt::torrent_status status = m_torrent.status();

    int dlLimit = m_torrent.download_limit();
    int ulLimit = m_torrent.upload_limit();
    int maxConnections = m_torrent.max_connections();
    int maxUploads = m_torrent.max_uploads();

    if (dlLimit < 0) { dlLimit = 0; }
    if (dlLimit > 0) { dlLimit /= 1024; }
    if (ulLimit < 0) { ulLimit = 0; }
    if (ulLimit > 0) { ulLimit /= 1024; }
    if (maxConnections >= 16777215) { maxConnections = -1; }
    if (maxUploads >= 16777215) { maxUploads = -1; }

    m_dlLimit.SetWindowText(std::to_wstring(dlLimit).c_str());
    m_ulLimit.SetWindowText(std::to_wstring(ulLimit).c_str());
    m_maxConnections.SetWindowText(std::to_wstring(maxConnections).c_str());
    m_maxUploads.SetWindowText(std::to_wstring(maxUploads).c_str());
    if (status.sequential_download) { m_sequentialDownload.Check(); }

    return FALSE;
}
