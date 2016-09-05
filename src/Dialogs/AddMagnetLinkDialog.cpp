#include "AddMagnetLinkDialog.hpp"

#include <regex>

#include <shellapi.h>
#include <strsafe.h>

#include <libtorrent/torrent_info.hpp>

#include "../Commands/FindMetadataCommand.hpp"
#include "../Scaler.hpp"
#include "../Translator.hpp"
#include "../UI/TextBox.hpp"
#include "../VersionInformation.hpp"

namespace lt = libtorrent;
using Dialogs::AddMagnetLinkDialog;

AddMagnetLinkDialog::AddMagnetLinkDialog()
{
}

AddMagnetLinkDialog::~AddMagnetLinkDialog()
{
}

std::vector<lt::torrent_info> AddMagnetLinkDialog::GetTorrentFiles()
{
    return m_torrent_files;
}

std::vector<std::string> AddMagnetLinkDialog::GetLinks()
{
    UI::TextBox magnetLinks = GetDlgItem(ID_MAGNET_LINKS_TEXT);

    std::vector<std::string> result;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    std::string l = magnetLinks.GetValueA();

    while ((pos = l.find('\n', prev)) != std::string::npos)
    {
        result.push_back(l.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    result.push_back(l.substr(prev));

    return result;
}

void AddMagnetLinkDialog::OnAddMagnetLinks(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    // Subscribe to notifications
    GetParent().SendMessage(PT_REGISTERNOTIFY, NULL, reinterpret_cast<LPARAM>(m_hWnd));

    auto links = GetLinks();
    m_waiting_for = (int)links.size();

    if (links.empty())
    {
        // Show error dialog
    }

    // Disable textbox and button
    CButton addButton = GetDlgItem(ID_MAGNET_ADD_LINKS);
    CProgressBarCtrl progress = GetDlgItem(ID_MAGNET_PROGRESS);
    UI::TextBox magnetLinks = GetDlgItem(ID_MAGNET_LINKS_TEXT);

    addButton.EnableWindow(FALSE);
    progress.SetMarquee(TRUE);
    magnetLinks.EnableWindow(FALSE);

    std::string magnet_prefix = "magnet:?xt=urn:btih:";

    for (auto& link : links)
    {
        link.erase(link.find_last_not_of("\r") + 1);
        if (link.empty()) { continue; }

        // If only info hash, append magnet link template
        if ((link.size() == 40 && !std::regex_match(link, std::regex("[^0-9A-Fa-f]")))
            || (link.size() == 32 && !std::regex_match(link, std::regex(""))))
        {
            link = magnet_prefix + link;
        }

        // Check if link starts with "magnet:?xt=urn:btih:"
        if (link.substr(0, magnet_prefix.size()) != magnet_prefix)
        {
            continue;
        }

        // Send a command to find magnet links
        Commands::FindMetadataCommand cmd{ link };
        GetParent().SendMessage(PT_FINDMETADATA, NULL, reinterpret_cast<LPARAM>(&cmd));
    }

    UpdateCount();
}

void AddMagnetLinkDialog::OnDestroy()
{
    // Unsubscribe
    GetParent().SendMessage(PT_UNREGISTERNOTIFY, NULL, reinterpret_cast<LPARAM>(m_hWnd));
}

void AddMagnetLinkDialog::OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    EndDialog(m_torrent_files.size() > 0 ? IDOK : nID);
}

BOOL AddMagnetLinkDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    CenterWindow();

    SetWindowText(TRW("add_magnet_link_s"));
    SetDlgItemText(ID_MAGNET_LINKS_GROUP, TRW("magnet_link_s"));
    SetDlgItemText(ID_MAGNET_ADD_LINKS, TRW("add_link_s"));

    return FALSE;
}

LRESULT AddMagnetLinkDialog::OnMetadataFound(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const lt::torrent_info* ti = reinterpret_cast<const lt::torrent_info*>(lParam);
    m_torrent_files.push_back(lt::torrent_info(*ti));

    UpdateCount();

    if (m_waiting_for == (int)m_torrent_files.size())
    {
        EndDialog(IDOK);
    }

    return FALSE;
}

void AddMagnetLinkDialog::UpdateCount()
{
    TCHAR text[1024];
    StringCchPrintf(text, ARRAYSIZE(text), TEXT("%d / %d"), m_torrent_files.size(), m_waiting_for);
    SetDlgItemText(ID_MAGNET_CURRENT_STATUS, text);
}
