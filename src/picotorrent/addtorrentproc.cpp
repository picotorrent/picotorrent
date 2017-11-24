#include "addtorrentproc.hpp"

#include "addtorrentdlg.hpp"
#include "magnetlinkvalidator.hpp"
#include "sessionstate.hpp"
#include "translator.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/filedlg.h>

const wxString MagnetPrefix = "magnet:?xt=urn:btih:";

namespace lt = libtorrent;
using pt::AddTorrentProcedure;

AddTorrentProcedure::AddTorrentProcedure(wxWindow* parent,
    std::shared_ptr<pt::Translator> translator,
    std::shared_ptr<pt::SessionState> state)
    : m_parent(parent),
    m_state(state),
    m_trans(translator)
{
}

void AddTorrentProcedure::Execute()
{
    // Open some torrent files!
    wxFileDialog openDialog(
        m_parent,
        i18n(m_trans, "add_torrent_s"),
        wxEmptyString,
        wxEmptyString,
        "Torrent files (*.torrent)|*.torrent",
        wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

    if (openDialog.ShowModal() != wxID_OK)
    {
        return;
    }

    wxArrayString paths;
    openDialog.GetPaths(paths);

    if (paths.IsEmpty())
    {
        return;
    }

    std::vector<lt::add_torrent_params> params;

    for (wxString& filePath : paths)
    {
        lt::add_torrent_params p;
        lt::error_code ec;

        p.ti = std::make_shared<lt::torrent_info>(filePath.ToStdString(), ec);
        p.file_priorities.resize(p.ti->num_files(), 4);

        if (ec)
        {
            continue;
        }

        params.push_back(p);
    }

    return Execute(params);
}

void AddTorrentProcedure::ExecuteMagnet()
{
    wxTextEntryDialog dlg(
        m_parent,
        i18n(m_trans, "magnet_link_s"),
        i18n(m_trans, "add_magnet_link_s"),
        wxEmptyString);

    dlg.SetMaxClientSize(dlg.GetMinClientSize());
    dlg.SetTextValidator(MagnetLinkValidator());

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    wxString magnetLink = dlg.GetValue();
    if (!magnetLink.StartsWith(MagnetPrefix)) { magnetLink = MagnetPrefix + magnetLink; }

    lt::add_torrent_params p;
    lt::error_code ec;
    lt::parse_magnet_uri(magnetLink.ToStdString(), p, ec);

    if (ec)
    {
        // TODO (log fatal error? this should be caught by the validator);
        return;
    }

    std::vector<lt::add_torrent_params> params{ p };
    return Execute(params);
}

void AddTorrentProcedure::Execute(std::vector<lt::add_torrent_params>& params)
{
    for (auto& param : params)
    {
        // TODO
        param.save_path = "";
    }

    AddTorrentDialog addDialog(m_parent, m_trans, params);
    int result = addDialog.ShowModal();

    if (result == wxID_OK)
    {
        for (lt::add_torrent_params& p : params)
        {
            m_state->session->async_add_torrent(p);
        }
    }
}
