#include "RemoveTorrentsController.hpp"

#include <commctrl.h>

#include <libtorrent/session.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_handle.hpp>

#include "../Commands/RemoveTorrentsCommand.hpp"
#include "../Configuration.hpp"
#include "../Models/Torrent.hpp"
#include "../Translator.hpp"

namespace lt = libtorrent;
using Controllers::RemoveTorrentsController;

RemoveTorrentsController::RemoveTorrentsController(HWND hWndParent, const std::shared_ptr<lt::session>& session, const std::map<lt::sha1_hash, lt::torrent_handle>& torrents)
    : m_hWndParent(hWndParent),
    m_session(session),
    m_torrents(torrents)
{
}

RemoveTorrentsController::~RemoveTorrentsController()
{
}

void RemoveTorrentsController::Execute(const std::vector<Models::Torrent>& torrents, bool removeData)
{
    Configuration& cfg = Configuration::GetInstance();

    bool prompt = cfg.GetPromptForRemovingData();

    if (prompt && removeData)
    {
        std::wstring content = TRW("confirm_remove_description");
        std::wstring mainInstruction = TRW("confirm_remove");
        std::wstring verificationText = TRW("do_not_ask_again");

        TASKDIALOGCONFIG tdf = { 0 };
        tdf.cbSize = sizeof(TASKDIALOGCONFIG);
        tdf.dwCommonButtons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
        tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW;
        tdf.hwndParent = m_hWndParent;
        tdf.pszContent = content.c_str();
        tdf.pszMainIcon = TD_WARNING_ICON;
        tdf.pszMainInstruction = mainInstruction.c_str();
        tdf.pszVerificationText = verificationText.c_str();
        tdf.pszWindowTitle = TEXT("PicoTorrent");

        int pnButton = 0;
        BOOL pfVerificationFlagChecked = FALSE;
        HRESULT hResult = TaskDialogIndirect(&tdf, &pnButton, NULL, &pfVerificationFlagChecked);

        if (hResult != S_OK)
        {
            // LOg
            return;
        }

        switch (pnButton)
        {
        case IDOK:
        {
            if (pfVerificationFlagChecked)
            {
                cfg.SetPromptForRemovingData(false);
            }
            break;
        }
        case IDCANCEL:
            return;
        }
    }

    int flags = removeData ? lt::session::options_t::delete_files : 0;

    for (auto& t : torrents)
    {
        const lt::torrent_handle& th = m_torrents.at(t.infoHash);
        m_session->remove_torrent(th, flags);
    }
}
