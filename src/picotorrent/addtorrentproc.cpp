#include "addtorrentproc.hpp"

#include "addtorrentdlg.hpp"
#include "config.hpp"
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
    std::shared_ptr<pt::Configuration> cfg,
    std::shared_ptr<pt::Translator> translator,
    std::shared_ptr<pt::SessionState> state)
    : m_parent(parent),
    m_cfg(cfg),
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

    return Execute(paths);
}

void AddTorrentProcedure::Execute(wxArrayString const& files)
{
    std::vector<lt::add_torrent_params> params;

    for (wxString const& filePath : files)
    {
        lt::add_torrent_params p;
        lt::error_code ec;
        p.ti = std::make_shared<lt::torrent_info>(std::string(filePath.ToUTF8()), ec);

        if (ec)
        {
            wxLogWarning("Could not parse torrent file: %s", ec.message());
            continue;
        }

        p.file_priorities.resize(p.ti->num_files(), lt::default_priority);

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
        wxLogFatalError("Total error when parsing magnet link %s", magnetLink);
        return;
    }

    std::vector<lt::add_torrent_params> params{ p };
    return Execute(params);
}

void AddTorrentProcedure::ExecuteMagnet(wxArrayString const& magnetLinks)
{
    std::vector<lt::add_torrent_params> params;

    for (wxString const& link : magnetLinks)
    {
        lt::add_torrent_params p;
        lt::error_code ec;

        lt::parse_magnet_uri(link.ToStdString(), p, ec);

        if (ec)
        {
            wxLogFatalError("Total error when parsing magnet link %s", link);
            return;
        }

        params.push_back(p);
    }

    return Execute(params);
}

void AddTorrentProcedure::Execute(std::vector<lt::add_torrent_params>& params)
{
    if (params.empty())
    {
        return;
    }

    for (auto& param : params)
    {
        param.flags |= lt::torrent_flags::duplicate_is_error;
        param.save_path = m_cfg->DefaultSavePath().string();
    }

    int result = wxID_OK;

    if (!m_cfg->UI()->SkipAddTorrentDialog())
    {
        AddTorrentDialog addDialog(m_parent, m_trans, params);
        result = addDialog.ShowModal();
    }

    if (result == wxID_OK)
    {
        for (lt::add_torrent_params& p : params)
        {
            m_state->session->async_add_torrent(p);
        }
    }
}
