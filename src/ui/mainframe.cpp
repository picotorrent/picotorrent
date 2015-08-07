#include "mainframe.h"

#include <libtorrent/alert_types.hpp>
#include <libtorrent/version.hpp>
#include <wx/filedlg.h>
#include <wx/menu.h>

#include "../common.h"

namespace lt = libtorrent;

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_SIZE(MainFrame::OnSize)

    EVT_MENU(MainFrame::ptID_FILE_ADD_TORRENT, MainFrame::OnFileAddTorrent)
    EVT_MENU(MainFrame::ptID_FILE_EXIT, MainFrame::OnFileExit)

    

    EVT_LIST_ITEM_ACTIVATED(1000, MainFrame::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(1000, MainFrame::OnListItemRightClick)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(lt::session_handle& session)
    : wxFrame(NULL, wxID_ANY, wxT("PicoTorrent")),
    session_(session)
{
    torrentList_ = new TorrentListCtrl(this,
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
    if (items_.find(status.info_hash) != items_.end())
    {
        // Log error
        return;
    }

    long idx = torrentList_->InsertItem(torrentList_->GetItemCount(), "temp");
    items_[status.info_hash] = idx;
    itemsReverse_[idx] = status.info_hash;

    UpdateTorrent(status);
}

void MainFrame::UpdateTorrent(const lt::torrent_status& status)
{
    auto item = items_.find(status.info_hash);

    if (item == items_.end())
    {
        // Log error
        return;
    }

    long idx = item->second;

    std::string queuePosition = "";

    if (status.queue_position >= 0)
    {
        queuePosition = std::to_string(status.queue_position + 1);
    }

    torrentList_->SetItem(idx, 0, status.name);
    torrentList_->SetItem(idx, 1, queuePosition);
    torrentList_->SetItem(idx, 2, GetTorrentState(status));
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
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFile.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    lt::add_torrent_params p;
    p.save_path = "C:/Downloads";
    p.ti = boost::make_shared<lt::torrent_info>(openFile.GetPath().ToStdString());

    session_.async_add_torrent(p);
}

void MainFrame::OnFileExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MainFrame::OnListItemActivated(wxListEvent& event)
{
    long idx = event.GetIndex();
    auto hash = itemsReverse_.find(idx);

    if (hash == itemsReverse_.end())
    {
        // Log error (selected item is not in our list of items)
        return;
    }
    
    lt::torrent_handle handle = session_.find_torrent(hash->second);

    if (!handle.is_valid())
    {
        // Log error (handle is invalid)
        return;
    }

    auto existingDetails = details_.find(hash->second);

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
    details->Bind(wxEVT_CLOSE_WINDOW, std::bind(&MainFrame::OnTorrentDetailsFrameClose, this, std::placeholders::_1, hash->second));

    // Store this frame
    details_[hash->second] = details;
}

void MainFrame::OnListItemRightClick(wxListEvent& event)
{
    long idx = event.GetIndex();
    auto hash = itemsReverse_.find(idx);

    if (hash == itemsReverse_.end())
    {
        // Log error
        return;
    }

    lt::torrent_handle handle = session_.find_torrent(hash->second);

    if (!handle.is_valid())
    {
        // Log error (invalid handle)
        return;
    }

    lt::torrent_status status = handle.status();

    wxMenu menu;
    menu.Append(wxID_ANY, wxT("Resume"))->Enable(!status.auto_managed && status.paused);
    menu.Append(wxID_ANY, wxT("Pause"))->Enable(!status.auto_managed && !status.paused);
    menu.AppendCheckItem(wxID_ANY, wxT("Auto managed"))->Check(status.auto_managed);
    menu.AppendSeparator();
    menu.Append(wxID_ANY, wxT("Force re-check"))->Enable(status.state != lt::torrent_status::checking_files);
    menu.Append(wxID_ANY, wxT("Move torrent"))->Enable(!status.moving_storage);
    menu.AppendSeparator();
    menu.Append(wxID_ANY, wxT("Remove"));
    menu.Append(wxID_ANY, wxT("Remove (and remove data)"));
    menu.AppendSeparator();
    menu.Append(wxID_ANY, wxT("Details"));

    PopupMenu(&menu, event.GetPoint());
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

