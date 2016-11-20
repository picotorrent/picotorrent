#include "AddMagnetLinkDialog.hpp"

#include <regex>
#include <sstream>

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

static std::string magnet_prefix = "magnet:?xt=urn:btih:";

AddMagnetLinkDialog::AddMagnetLinkDialog(const std::vector<std::wstring>& magnetLinks)
    : m_links(magnetLinks)
{
}

AddMagnetLinkDialog::~AddMagnetLinkDialog()
{
}

std::vector<lt::torrent_info> AddMagnetLinkDialog::GetTorrentFiles()
{
    return m_torrent_files;
}

bool AddMagnetLinkDialog::IsInfoHash(std::string const& link)
{
    return (link.size() == 40 && !std::regex_match(link, std::regex("[^0-9A-Fa-f]")))
        || (link.size() == 32 && !std::regex_match(link, std::regex("")));
}

bool AddMagnetLinkDialog::IsMagnetLink(std::string const& link)
{
    return link.substr(0, magnet_prefix.size()) == magnet_prefix;
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
        std::string link = l.substr(prev, pos - prev);
        link.erase(link.find_last_not_of("\r") + 1);
        link.erase(link.find_last_not_of("\n") + 1);

        // If only info hash, append magnet link template
        if (IsInfoHash(link))
        {
            link = magnet_prefix + link;
        }

        // Check if link starts with "magnet:?xt=urn:btih:"
        if (IsMagnetLink(link))
        {
            result.push_back(l.substr(prev, pos - prev));
        }

        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    std::string ll = l.substr(prev);
    ll.erase(ll.find_last_not_of("\r") + 1);
    ll.erase(ll.find_last_not_of("\n") + 1);

    if (IsInfoHash(ll))
    {
        ll = magnet_prefix + ll;
    }

    if (IsMagnetLink(ll))
    {
        result.push_back(ll);
    }

    return result;
}

void AddMagnetLinkDialog::OnAddMagnetLinks(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    auto links = GetLinks();
    m_waiting_for = (int)links.size();

    if (links.empty())
    {
        TaskDialog(
            m_hWnd,
            NULL,
            TEXT("PicoTorrent"),
            TRW("no_magnet_links_found"),
            NULL,
            TDCBF_OK_BUTTON,
            TD_WARNING_ICON,
            NULL);
        return;
    }

    // Subscribe to notifications
    GetParent().SendMessage(PT_REGISTERNOTIFY, NULL, reinterpret_cast<LPARAM>(m_hWnd));

    // Disable textbox and button
    CButton addButton = GetDlgItem(ID_MAGNET_ADD_LINKS);
    CProgressBarCtrl progress = GetDlgItem(ID_MAGNET_PROGRESS);
    UI::TextBox magnetLinks = GetDlgItem(ID_MAGNET_LINKS_TEXT);

    addButton.EnableWindow(FALSE);
    progress.SetMarquee(TRUE);
    magnetLinks.EnableWindow(FALSE);

    for (auto& link : links)
    {
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

    std::wstringstream wss;
    for (auto& l : m_links) { wss << l << std::endl; }
    SetDlgItemText(ID_MAGNET_LINKS_TEXT, wss.str().c_str());

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
