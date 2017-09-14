#include "AddTorrentController.hpp"

#include <fstream>
#include <sstream>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../CommandLine.hpp"
#include "../Configuration.hpp"
#include "../Dialogs/AddTorrentDialog.hpp"
#include "../Dialogs/OpenFileDialog.hpp"
#include "../Log.hpp"
#include "../Translator.hpp"

const GUID DLG_OPEN = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x60 } };
const GUID DLG_SAVE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x61 } };

namespace lt = libtorrent;
using Controllers::AddTorrentController;

AddTorrentController::AddTorrentController(HWND hWndOwner,
    const std::shared_ptr<lt::session>& session)
    : m_hWndOwner(hWndOwner),
    m_session(session)
{
}

void AddTorrentController::Execute()
{
    std::vector<std::wstring> res = OpenFiles();

    if (res.empty())
    {
        return;
    }

    Execute(res);
}

void AddTorrentController::Execute(std::vector<lt::add_torrent_params>& params)
{
	if (params.empty())
	{
		return;
	}

	Configuration& cfg = Configuration::GetInstance();

	for (lt::add_torrent_params& p : params)
	{
		// Set default settings
		p.save_path = cfg.GetDefaultSavePath();

		if (!p.ti)
		{
			continue;
		}

		lt::torrent_handle th = m_session->find_torrent(p.ti->info_hash());

		if (th.is_valid())
		{
			LOG(Warning) << "Torrent " << p.ti->name() << " already in session.";
			TaskDialog(
				m_hWndOwner,
				NULL,
				TEXT("PicoTorrent"),
				TRW("torrent_s_already_in_session"),
				TWS(p.ti->name()),
				TDCBF_OK_BUTTON,
				TD_WARNING_ICON,
				NULL);
			continue;
		}
	}

	bool shouldAddFiles = true;
	Dialogs::AddTorrentDialog dlg(params);

	if (cfg.UI()->GetShowAddTorrentDialog())
	{
		shouldAddFiles = (dlg.DoModal(m_hWndOwner) == IDOK);
	}

	if (shouldAddFiles)
	{
		for (auto& p : params)
		{
			m_session->async_add_torrent(p);
		}
	}
}

void AddTorrentController::Execute(const std::vector<std::wstring>& files)
{
    std::vector<lt::add_torrent_params> params;

    for (auto& path : files)
    {
        std::ifstream input(path, std::ios::binary);
        std::stringstream ss;
        ss << input.rdbuf();
        std::string buf = ss.str();

        lt::error_code ltec;
        lt::bdecode_node node;
        lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ltec);

        if (ltec)
        {
            // LOG
            continue;
        }

		lt::add_torrent_params param;
		param.ti = std::make_shared<lt::torrent_info>(node, ltec);

        if (ltec)
        {
            continue;
        }

        params.push_back(param);
    }

    Execute(params);
}

void AddTorrentController::ExecuteMagnets(const std::vector<std::wstring>& magnetLinks)
{
    Configuration& cfg = Configuration::GetInstance();
    std::vector<lt::add_torrent_params> params;

    for (auto& link : magnetLinks)
    {
        libtorrent::error_code ec;
        lt::add_torrent_params p;
        lt::parse_magnet_uri(TS(link), p, ec);

        if (ec)
        {
            // LOG
            continue;
        }

        params.push_back(p);
    }

	return Execute(params);
}

std::vector<std::wstring> AddTorrentController::OpenFiles()
{
    COMDLG_FILTERSPEC fileTypes[] =
    {
        { TEXT("Torrent files"), TEXT("*.torrent") },
        { TEXT("All files"), TEXT("*.*") }
    };

    Dialogs::OpenFileDialog openDialog;
    openDialog.SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
    openDialog.SetGuid(DLG_OPEN);
    openDialog.SetOptions(openDialog.GetOptions() | FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST);
    openDialog.Show();

    return openDialog.GetPaths();
}
