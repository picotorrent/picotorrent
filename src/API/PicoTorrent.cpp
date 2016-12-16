#include "PicoTorrent.hpp"

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include "../Commands/InvokeCommand.hpp"
#include "../Configuration.hpp"
#include "IO/FileSystem.hpp"
#include "LoggerProxy.hpp"
#include "../resources.h"
#include "../StringUtils.hpp"
#include "TranslatorProxy.hpp"
#include "../VersionInformation.hpp"

#define EXT_MENUITEM_ID_START 10000

using API::PicoTorrent;

PicoTorrent::PicoTorrent(HWND hWndOwner, std::shared_ptr<libtorrent::session> session)
    : m_hWndOwner(hWndOwner),
    m_session(session),
    m_threadId(std::this_thread::get_id())
{
    SetWindowSubclass(
        m_hWndOwner,
        &PicoTorrent::SubclassProc,
        1338,
        reinterpret_cast<DWORD_PTR>(this));
}

void PicoTorrent::EmitTorrentAdded(Torrent const& torrent)
{
    for (auto& sink : m_torrentSinks)
    {
        sink->OnTorrentAdded(torrent);
    }
}

void PicoTorrent::EmitTorrentFinished(Torrent const& torrent)
{
    for (auto& sink : m_torrentSinks)
    {
        sink->OnTorrentFinished(torrent);
    }
}

void PicoTorrent::EmitTorrentRemoved(libtorrent::sha1_hash const& infoHash)
{
    std::stringstream ss;
    ss << infoHash;
    std::string hash = ss.str();

    for (auto& sink : m_torrentSinks)
    {
        sink->OnTorrentRemoved(hash);
    }
}

void PicoTorrent::EmitTorrentUpdated(std::vector<Torrent> const& torrents)
{
	for (auto& sink : m_torrentSinks)
	{
		sink->OnTorrentUpdated(torrents);
	}
}

void PicoTorrent::AddMenuItem(MenuItem const& item)
{
    HMENU hMenu = GetMenu(m_hWndOwner);
    HMENU ext = GetSubMenu(hMenu, 2);

    int id = EXT_MENUITEM_ID_START + (int)m_menuItems.size();
    AppendMenu(ext, MF_STRING, id, item.text.c_str());

    m_menuItems.insert({ id, item });
}

std::shared_ptr<picojson::object> PicoTorrent::GetConfiguration()
{
    return Configuration::GetInstance().GetRawObject();
}

std::string PicoTorrent::GetCurrentVersion()
{
    return VersionInformation::GetCurrentVersion();
}

std::shared_ptr<IFileSystem> PicoTorrent::GetFileSystem()
{
    return std::make_shared<IO::FileSystem>();
}

std::shared_ptr<ILogger> PicoTorrent::GetLogger()
{
    return std::make_shared<API::LoggerProxy>();
}

std::shared_ptr<libtorrent::session> PicoTorrent::GetSession()
{
    return m_session;
}

std::shared_ptr<ITranslator> PicoTorrent::GetTranslator()
{
    return std::make_shared<TranslatorProxy>();
}

void PicoTorrent::RegisterEventSink(std::shared_ptr<ITorrentEventSink> sink)
{
    m_torrentSinks.push_back(sink);
}

std::unique_ptr<TaskDialogResult> PicoTorrent::ShowTaskDialog(TASKDIALOGCONFIG* tdcfg)
{
    if (std::this_thread::get_id() != m_threadId)
    {
        // Invoke
        Commands::InvokeCommand cmd;
        std::unique_ptr<TaskDialogResult> result;
        cmd.callback = [this, &result, &tdcfg]() { result = ShowTaskDialog(tdcfg); };

        SendMessage(m_hWndOwner, PT_INVOKE, NULL, reinterpret_cast<LPARAM>(&cmd));

        return std::move(result);
    }

    tdcfg->hwndParent = m_hWndOwner;

    int pnButton = -1;
    int pnRadioButton = -1;
    BOOL pfVerificationFlagChecked = FALSE;

    TaskDialogIndirect(tdcfg, &pnButton, &pnRadioButton, &pfVerificationFlagChecked);

    auto result = std::make_unique<TaskDialogResult>();
    result->button = pnButton;
    result->radioButton = pnRadioButton;
    result->verificationChecked = pfVerificationFlagChecked == TRUE;

    return std::move(result);
}

LRESULT PicoTorrent::SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    PicoTorrent* pico = reinterpret_cast<PicoTorrent*>(dwRefData);

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        DWORD dwId = LOWORD(wParam);

        if (pico->m_menuItems.find(dwId) != pico->m_menuItems.end())
        {
            pico->m_menuItems.at(dwId).onClick();
            return 0;
        }

        break;
    };
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
