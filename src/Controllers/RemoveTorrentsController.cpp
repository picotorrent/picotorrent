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

RemoveTorrentsController::RemoveTorrentsController(const std::shared_ptr<lt::session>& session, const std::map<lt::sha1_hash, lt::torrent_handle>& torrents)
    : m_session(session),
    m_torrents(torrents)
{
}

RemoveTorrentsController::~RemoveTorrentsController()
{
}

void RemoveTorrentsController::Execute(HWND hWndParent, const Commands::RemoveTorrentsCommand& cmd)
{
    Configuration& cfg = Configuration::GetInstance();

    bool prompt = cfg.GetPromptForRemovingData();
    bool shouldRemove = !prompt;

    if (prompt)
    {
        std::wstring content = TRW("confirm_remove_description");
        std::wstring mainInstruction = TRW("confirm_remove");
        std::wstring verificationText = TRW("do_not_ask_again");

        TASKDIALOGCONFIG tdf = { 0 };
        tdf.cbSize = sizeof(TASKDIALOGCONFIG);
        tdf.dwCommonButtons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
        tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW;
        tdf.hwndParent = hWndParent;
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

            shouldRemove = true;
            break;
        }
        }
    }

    if (!shouldRemove) { return; }

    int flags = cmd.removeData ? lt::session::options_t::delete_files : 0;

    for (auto& t : cmd.torrents)
    {
        const lt::torrent_handle& th = m_torrents.at(t.infoHash);
        m_session->remove_torrent(th, flags);
    }
}
