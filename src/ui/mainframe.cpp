#include "mainframe.h"

#include <libtorrent/alert_types.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/version.hpp>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/menu.h>

#include "../common.h"
#include "addtorrentdialog.h"

namespace lt = libtorrent;

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_SIZE(MainFrame::OnSize)

    EVT_MENU(ptID_FILE_ADD_TORRENT, MainFrame::OnFileAddTorrent)
    EVT_MENU(ptID_FILE_EXIT, MainFrame::OnFileExit)
    EVT_MENU(wxID_ANY, MainFrame::OnTorrentContextMenu)

    EVT_LIST_ITEM_ACTIVATED(1000, MainFrame::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(1000, MainFrame::OnListItemRightClick)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
    : wxFrame(NULL, wxID_ANY, wxT("PicoTorrent"))
{
    SetIcon(wxIcon("progicon"));

    torrentList_ = new wxListCtrl(this,
        1000,
        wxDefaultPosition,
        wxDefaultSize,
        wxLC_REPORT);

    wxListItem col;

    col.SetAlign(wxListColumnFormat::wxLIST_FORMAT_LEFT);
    col.SetText("Name");
    col.SetWidth(200);
    torrentList_->InsertColumn(0, col);
    
    col.SetAlign(wxListColumnFormat::wxLIST_FORMAT_RIGHT);
    col.SetText("#");
    col.SetWidth(30);
    torrentList_->InsertColumn(1, col);

    col.SetAlign(wxListColumnFormat::wxLIST_FORMAT_LEFT);
    col.SetText("Status");
    col.SetWidth(150);
    torrentList_->InsertColumn(2, col);

    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ptID_FILE_ADD_TORRENT, wxT("Add torrent"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ptID_FILE_EXIT, wxT("Exit"));

    wxMenuBar* mainMenu = new wxMenuBar();
    mainMenu->Append(fileMenu, wxT("File"));
    SetMenuBar(mainMenu);

    const int widths[2] = { -2, -1 };

    CreateStatusBar(2);
    SetStatusWidths(2, widths);
    SetStatusText(wxString::Format("libtorrent v%s", LIBTORRENT_VERSION), 1);
}

void MainFrame::AddTorrent(const lt::torrent_status& status)
{
    wxListItem item;
    item.SetData(new lt::sha1_hash(status.info_hash));
    item.SetId(torrentList_->GetItemCount());

    torrentList_->InsertItem(item);

    UpdateTorrents(std::map<lt::sha1_hash, lt::torrent_status>{{ status.info_hash, status }});
}

void MainFrame::UpdateTorrents(std::map<libtorrent::sha1_hash, libtorrent::torrent_status> status)
{
    torrentList_->Freeze();

    long idx = -1;

    while ((idx = torrentList_->GetNextItem(idx, wxLIST_NEXT_ALL)) != wxNOT_FOUND)
    {
        wxListItem item;
        item.SetId(idx);

        torrentList_->GetItem(item);

        lt::sha1_hash* hash = (lt::sha1_hash*)item.GetData();
        lt::torrent_status& st = status[*hash];

        std::string queuePosition = "";

        if (st.queue_position >= 0)
        {
            queuePosition = std::to_string(st.queue_position + 1);
        }

        torrentList_->SetItem(idx, 0, st.name);
        torrentList_->SetItem(idx, 1, queuePosition);
        torrentList_->SetItem(idx, 2, GetTorrentState(st));
    }

    torrentList_->Thaw();
}

void MainFrame::RemoveTorrent(const lt::sha1_hash& hash)
{
    long idx = -1;

    while ((idx = torrentList_->GetNextItem(idx, wxLIST_NEXT_ALL)) != wxNOT_FOUND)
    {
        wxListItem item;
        item.SetId(idx);

        torrentList_->GetItem(item);

        lt::sha1_hash* storedHash = (lt::sha1_hash*)item.GetData();

        if (hash == *storedHash)
        {
            break;
        }
    }

    torrentList_->DeleteItem(idx);

    // TODO: Loop through each item with an index greater than 'idx'
    // and subtract 1.

    // Close details window (if open)
    if (details_.find(hash) != details_.end())
    {
        details_[hash]->Destroy();
        details_.erase(hash);
    }
}

void MainFrame::OnSize(wxSizeEvent& event)
{
    wxSize size = GetClientSize();
    torrentList_->SetSize(0, 0, size.x, size.y);
}

void MainFrame::OnFileAddTorrent(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog openFile(this,
        wxT("Open file"),
        "",
        "",
        "Torrent files (*.torrent)|*.torrent|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

    if (openFile.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    wxArrayString paths;
    openFile.GetPaths(paths);

    std::vector<boost::shared_ptr<lt::torrent_info>> torrents;
    torrents.reserve(paths.GetCount());

    for (wxString& path : paths)
    {
        lt::error_code ec;
        //boost::shared_ptr<lt::torrent_info> torrent = boost::make_shared<lt::torrent_info>(path.ToStdString(), ec);

        if (ec)
        {
            // Log error
            continue;
        }

        //torrents.push_back(torrent);
    }

    //AddTorrentDialog* dlg = new AddTorrentDialog(this, session_, torrents);
    //dlg->Show(true);
}

void MainFrame::OnFileExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MainFrame::OnTorrentContextMenu(wxCommandEvent& event)
{
    long idx = -1;
    /*
    while ((idx = torrentList_->GetNextItem(idx, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != wxNOT_FOUND)
    {
        wxListItem item;
        item.SetId(idx);

        torrentList_->GetItem(item);

        lt::sha1_hash* hash = (lt::sha1_hash*)item.GetData();
        lt::torrent_handle handle = session_.find_torrent(*hash);

        switch (event.GetId())
        {
        case ptID_TORRENT_RESUME:
            handle.resume();
            break;

        case ptID_TORRENT_PAUSE:
            handle.pause();
            break;

        case ptID_TORRENT_AUTO_MANAGE_TOGGLE:
        {
            lt::torrent_status st = handle.status(0);
            handle.auto_managed(!st.auto_managed);
        }
            break;

        case ptID_TORRENT_FORCE_RECHECK:
            handle.force_recheck();
            break;

        case ptID_TORRENT_MOVE:
        {
            lt::torrent_status st = handle.status();
            wxDirDialog dlg(this, "Select new path", st.save_path);

            if (dlg.ShowModal() == wxID_OK)
            {
                handle.move_storage(dlg.GetPath().ToStdString());
            }
        }
            break;

        case ptID_TORRENT_REMOVE:
            session_.remove_torrent(handle);
            break;

        case ptID_TORRENT_REMOVE_DATA:
            session_.remove_torrent(handle, lt::session_handle::delete_files);
            break;

        case ptID_TORRENT_SHOW_DETAILS:
            ShowDetails(*hash);
            break;
        }
    }
    */
}

void MainFrame::ShowDetails(const lt::sha1_hash& hash)
{
    /*
    lt::torrent_handle handle = session_.find_torrent(hash);

    if (!handle.is_valid())
    {
        // Log error (handle is invalid)
        return;
    }

    auto existingDetails = details_.find(hash);

    if (existingDetails != details_.end())
    {
        existingDetails->second->Raise();
        return;
    }

    lt::torrent_status status = handle.status();

    // Show details frame
    TorrentDetailsFrame* details = new TorrentDetailsFrame(this, wxID_ANY);
    details->SetTorrent(status);
    details->Show(true);

    // Bind the close event to a handler which removes the mapping 
    details->Bind(wxEVT_CLOSE_WINDOW, std::bind(&MainFrame::OnTorrentDetailsFrameClose, this, std::placeholders::_1, hash));

    // Store this frame
    details_[hash] = details;
    */
}

void MainFrame::OnListItemActivated(wxListEvent& event)
{
    long idx = event.GetIndex();

    wxListItem item;
    item.SetId(idx);

    torrentList_->GetItem(item);

    lt::sha1_hash* hash = (lt::sha1_hash*)item.GetData();
    ShowDetails(*hash);
}

void MainFrame::OnListItemRightClick(wxListEvent& event)
{
    long idx = event.GetIndex();

    wxListItem item;
    item.SetId(idx);

    torrentList_->GetItem(item);

    lt::sha1_hash* hash = (lt::sha1_hash*)item.GetData();
    /*lt::torrent_handle handle = session_.find_torrent(*hash);

    if (!handle.is_valid())
    {
        // Log error (invalid handle)
        return;
    }

    lt::torrent_status status = handle.status();

    wxMenu menu;
    menu.Append(ptID_TORRENT_RESUME, wxT("Resume"))->Enable(!status.auto_managed && status.paused);
    menu.Append(ptID_TORRENT_PAUSE, wxT("Pause"))->Enable(!status.auto_managed && !status.paused);
    menu.AppendCheckItem(ptID_TORRENT_AUTO_MANAGE_TOGGLE, wxT("Auto managed"))->Check(status.auto_managed);
    menu.AppendSeparator();
    menu.Append(ptID_TORRENT_FORCE_RECHECK, wxT("Force re-check"))->Enable(status.state != lt::torrent_status::checking_files);
    menu.Append(ptID_TORRENT_MOVE, wxT("Move torrent"))->Enable(!status.moving_storage);
    menu.AppendSeparator();
    menu.Append(ptID_TORRENT_REMOVE, wxT("Remove"));
    menu.Append(ptID_TORRENT_REMOVE_DATA, wxT("Remove (and remove data)"));
    menu.AppendSeparator();
    menu.Append(ptID_TORRENT_SHOW_DETAILS, wxT("Details"));

    PopupMenu(&menu);
    */
}

void MainFrame::OnTorrentDetailsFrameClose(wxCloseEvent& event, lt::sha1_hash hash)
{
    details_.erase(hash);
    event.Skip();
}

wxString MainFrame::GetTorrentState(const lt::torrent_status& status)
{
    switch (status.state)
    {
    case lt::torrent_status::state_t::allocating:
        return "Allocating files";

    case lt::torrent_status::state_t::checking_files:
    case lt::torrent_status::state_t::checking_resume_data:
        return wxString::Format("Checking (%.2f%%)", status.progress * 100);

    case lt::torrent_status::state_t::downloading:
        return wxString::Format("Downloading (%.2f%%)", status.progress * 100);

    case lt::torrent_status::state_t::downloading_metadata:
        return "Downloading metadata";

    case lt::torrent_status::state_t::finished:
    case lt::torrent_status::state_t::seeding:
    {
        if (status.paused)
        {
            return "Finished";
        }

        return "Seeding";
    }
    break;
    }

    return "<unknown state>";
}

