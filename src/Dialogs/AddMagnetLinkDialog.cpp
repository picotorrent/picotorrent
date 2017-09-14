#include "AddMagnetLinkDialog.hpp"

#include <regex>
#include <sstream>

#include <shellapi.h>
#include <strsafe.h>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
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

bool AddMagnetLinkDialog::IsInfoHash(std::string const& link)
{
    return (link.size() == 40 && !std::regex_match(link, std::regex("[^0-9A-Fa-f]")))
        || (link.size() == 32 && !std::regex_match(link, std::regex("")));
}

bool AddMagnetLinkDialog::IsMagnetLink(std::string const& link)
{
    return link.substr(0, magnet_prefix.size()) == magnet_prefix;
}

std::vector<lt::add_torrent_params> AddMagnetLinkDialog::GetTorrentParams()
{
	return m_params;
}

std::vector<lt::add_torrent_params> AddMagnetLinkDialog::ParseTorrentParams()
{
	UI::TextBox magnetLinks = GetDlgItem(ID_MAGNET_LINKS_TEXT);

	std::vector<lt::add_torrent_params> result;

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

		lt::add_torrent_params params;
		lt::error_code ec;

		lt::parse_magnet_uri(link, params, ec);

		if (!ec)
		{
			result.push_back(params);
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
		lt::add_torrent_params params;
		lt::error_code ec;

		lt::parse_magnet_uri(ll, params, ec);

		if (!ec)
		{
			result.push_back(params);
		}
	}

	return result;
}

void AddMagnetLinkDialog::OnAddMagnetLinks(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_params = ParseTorrentParams();

    if (m_params.empty())
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

	EndDialog(IDOK);
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
