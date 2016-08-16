#include "AddTorrentController.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../Dialogs/AddTorrentDialog.hpp"
#include "../Dialogs/OpenFileDialog.hpp"
#include "../IO/File.hpp"
#include "../Translator.hpp"

const GUID DLG_OPEN = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x60 } };
const GUID DLG_SAVE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x61 } };

namespace lt = libtorrent;
using Controllers::AddTorrentController;

AddTorrentController::AddTorrentController(
    const std::shared_ptr<lt::session>& session)
    : m_session(session)
{
}

void AddTorrentController::Execute()
{
    std::vector<std::wstring> res = OpenFiles();

    if (res.empty())
    {
        return;
    }

    std::vector<std::shared_ptr<lt::add_torrent_params>> params;
    
    for (auto& path : res)
    {
        std::error_code ec;
        std::vector<char> buf = IO::File::ReadAllBytes(path, ec);

        if (ec)
        {
            // LOG
            continue;
        }

        lt::error_code ltec;
        lt::bdecode_node node;
        lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ltec);

        if (ltec)
        {
            // LOG
            continue;
        }

        auto p = std::make_shared<lt::add_torrent_params>();
        p->ti = boost::make_shared<lt::torrent_info>(node);

        params.push_back(p);
    }

    Dialogs::AddTorrentDialog dlg(params);

    switch (dlg.DoModal())
    {
    case IDOK:
    {
        for (auto& p : dlg.GetParams())
        {
            m_session->async_add_torrent(*p);
        }
        break;
    }
    }
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
