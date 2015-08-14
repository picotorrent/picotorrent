#include "addtorrentdialog.h"

#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dirdlg.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/textdlg.h>

#include "../common.h"
#include "../config.h"

namespace lt = libtorrent;

wxBEGIN_EVENT_TABLE(AddTorrentDialog, wxFrame)
    EVT_BUTTON(wxID_SAVEAS, AddTorrentDialog::OnBrowseSavePath)
    EVT_BUTTON(wxID_ADD, AddTorrentDialog::OnAdd)
    EVT_COMBOBOX(ptID_TORRENTS_COMBO, AddTorrentDialog::OnComboSelected)
    EVT_LIST_ITEM_RIGHT_CLICK(ptID_TORRENT_FILES_LIST, AddTorrentDialog::OnFileItemRightClick)
    EVT_MENU(wxID_ANY, AddTorrentDialog::OnMenu)
    EVT_TEXT(ptID_SAVE_PATH, AddTorrentDialog::OnSavePathChanged)
wxEND_EVENT_TABLE()

AddTorrentDialog::AddTorrentDialog(wxWindow* parent,
    lt::session_handle& session,
    std::vector<boost::shared_ptr<lt::torrent_info>> torrents)
    : wxFrame(parent, wxID_ANY, wxT("Add torrent(s)"), wxDefaultPosition, wxSize(400, 485), wxCAPTION | wxCLOSE_BOX),
    session_(session)
{
    Config& cfg = Config::GetInstance();
    std::string savePath = cfg.GetDefaultSavePath();

    for (torrent_info_ptr torrent : torrents)
    {
        lt::add_torrent_params p;
        p.save_path = savePath;
        p.ti = torrent;

        p.file_priorities.resize((size_t)torrent->num_files());
        std::fill(p.file_priorities.begin(), p.file_priorities.end(), 1);

        params_[torrent->info_hash()] = p;
    }

    panel_ = new wxPanel(this, wxID_ANY);

    InitTorrentsGroup();
    InitStorageGroup();

    new wxButton(panel_, wxID_ADD, "Add torrent(s)", wxPoint(300, 420), wxSize(90, 30));

    torrentsCombo_->Select(0);
    ShowTorrentInfo(0);
}

void AddTorrentDialog::InitTorrentsGroup()
{
    wxStaticBox* torrentGroup = new wxStaticBox(panel_, wxID_ANY, wxT("Torrent"), wxPoint(5, 5), wxSize(385, 150));

    wxStaticText* torrentName = new wxStaticText(torrentGroup, wxID_ANY, wxT("Name"), wxPoint(10, 25));
    torrentsCombo_ = new wxComboBox(torrentGroup, ptID_TORRENTS_COMBO, wxEmptyString, wxPoint(120, 20), wxSize(260, -1), wxArrayString(), wxCB_READONLY);

    if (params_.size() > 1)
    {
        torrentsCombo_->Insert("All torrents", 0);
    }

    for (auto& pair : params_)
    {
        int idx = torrentsCombo_->Insert(pair.second.ti->name(), torrentsCombo_->GetCount());
        items_[idx] = pair.second.ti->info_hash();
    }

    // Size
    wxStaticText* sizeLabel = new wxStaticText(torrentGroup, wxID_ANY, wxT("Size"), wxPoint(10, 50));
    sizeText_ = new wxStaticText(torrentGroup, wxID_ANY, wxEmptyString, wxPoint(120, 50));

    // Comment
    wxStaticText* commentLabel = new wxStaticText(torrentGroup, wxID_ANY, wxT("Comment"), wxPoint(10, 75));
    commentText_ = new wxStaticText(torrentGroup, wxID_ANY, wxEmptyString, wxPoint(120, 75));

    // Creation date
    wxStaticText* dateLabel = new wxStaticText(torrentGroup, wxID_ANY, wxT("Creation date"), wxPoint(10, 100));
    dateText_ = new wxStaticText(torrentGroup, wxID_ANY, wxEmptyString, wxPoint(120, 100));

    // Creator
    wxStaticText* creatorLabel = new wxStaticText(torrentGroup, wxID_ANY, wxT("Creator"), wxPoint(10, 125));
    creatorText_ = new wxStaticText(torrentGroup, wxID_ANY, wxEmptyString, wxPoint(120, 125));
}

void AddTorrentDialog::InitStorageGroup()
{
    wxStaticBox* storageGroup = new wxStaticBox(panel_, wxID_ANY, wxT("Storage"), wxPoint(5, 170), wxSize(385, 250));

    // save path
    wxStaticText* savePathLabel = new wxStaticText(storageGroup, wxID_ANY, wxT("Path"), wxPoint(10, 25));
    savePathText_ = new wxTextCtrl(storageGroup, ptID_SAVE_PATH, wxEmptyString, wxPoint(120, 20), wxSize(230, 22));

    // change save path
    wxButton* browseSavePath = new wxButton(storageGroup, wxID_SAVEAS, wxT("..."), wxPoint(350, 19), wxSize(30, 24));

    // files
    fileList_ = new wxListCtrl(storageGroup,
        ptID_TORRENT_FILES_LIST,
        wxPoint(10, 50),
        wxSize(370, 190),
        wxLC_REPORT | wxBORDER);

    wxListItem col;

    col.SetAlign(wxListColumnFormat::wxLIST_FORMAT_LEFT);
    col.SetText("Name");
    col.SetWidth(210);
    fileList_->InsertColumn(0, col);

    col.SetAlign(wxListColumnFormat::wxLIST_FORMAT_RIGHT);
    col.SetText("Size");
    col.SetWidth(80);
    fileList_->InsertColumn(1, col);

    col.SetAlign(wxListColumnFormat::wxLIST_FORMAT_LEFT);
    col.SetText("Priority");
    col.SetWidth(80);
    fileList_->InsertColumn(2, col);
}

void AddTorrentDialog::OnComboSelected(wxCommandEvent& event)
{
    int idx = torrentsCombo_->GetSelection();
    ShowTorrentInfo(idx);
}

void AddTorrentDialog::ShowTorrentInfo(int index)
{
    size_t totalSize = 0;
    std::string comment = "-";
    std::string creationDate = "-";
    std::string creator = "-";
    std::string savePath = "<multiple>";

    fileList_->DeleteAllItems();
    fileList_->Disable();

    if (params_.size() > 1 && index == 0)
    {
        // We selected "All torrents"
        for (auto& item : params_)
        {
            torrent_info_ptr torrent = item.second.ti;
            totalSize += torrent->total_size();
        }
    }
    else
    {
        lt::sha1_hash hash = items_[index];
        lt::add_torrent_params& p = params_[hash];
        torrent_info_ptr torrent = p.ti;

        totalSize = torrent->total_size();
        comment = torrent->comment();
        creator = torrent->creator();
        savePath = p.save_path;

        if (torrent->creation_date())
        {
            char fd[100];
            std::time_t tm(torrent->creation_date().value());
            std::strftime(fd, sizeof(fd), "%c", std::localtime(&tm));
            creationDate = fd;
        }

        // Update storage
        const lt::file_storage& storage = torrent->files();

        for (int i = 0; i < storage.num_files(); i++)
        {
            int idx = fileList_->InsertItem(fileList_->GetItemCount(), storage.file_name(i));
            fileList_->SetItem(idx, 1, Common::ToFileSize(storage.file_size(i)));
            fileList_->SetItem(idx, 2, PriorityString(p.file_priorities[i]));
        }

        fileList_->Enable();
    }

    // Update torrent information
    sizeText_->SetLabel(Common::ToFileSize(totalSize));
    commentText_->SetLabel(comment);
    dateText_->SetLabel(creationDate);
    creatorText_->SetLabel(creator);
    savePathText_->SetLabel(savePath);
}

void AddTorrentDialog::OnBrowseSavePath(wxCommandEvent& WXUNUSED(event))
{
    int idx = torrentsCombo_->GetSelection();
    std::string initialPath = "";

    if (params_.size() > 1 && idx > 0)
    {
        lt::sha1_hash hash = items_[idx - 1];
        lt::add_torrent_params& p = params_[hash];
        initialPath = p.save_path;
    }

    wxDirDialog dlg(this, wxT("Browse"), initialPath);

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    if (params_.size() > 1 && idx > 0)
    {
        lt::sha1_hash hash = items_[idx - 1];
        lt::add_torrent_params& p = params_[hash];
        p.save_path = dlg.GetPath().ToStdString();
    }
    else
    {
        for (auto& item : params_)
        {
            item.second.save_path = dlg.GetPath().ToStdString();
        }
    }

    savePathText_->SetLabel(dlg.GetPath());
}

void AddTorrentDialog::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    for (auto& item : params_)
    {
        session_.async_add_torrent(item.second);
    }

    Destroy();
}

void AddTorrentDialog::OnFileItemRightClick(wxListEvent& event)
{
    lt::sha1_hash hash = items_[torrentsCombo_->GetSelection()];
    lt::add_torrent_params& p = params_[hash];
    int idx = (int)event.GetIndex();
    bool isMulti = (fileList_->GetSelectedItemCount() > 1);

    int currentPriority = p.file_priorities[idx];

    wxMenu* prio = new wxMenu();
    prio->AppendCheckItem(ptID_PRIO_MAX, PriorityString(PRIO_MAX))->Check(currentPriority == PRIO_MAX);
    prio->AppendCheckItem(ptID_PRIO_HIGH, PriorityString(PRIO_HIGH))->Check(currentPriority == PRIO_HIGH);
    prio->AppendCheckItem(ptID_PRIO_NORMAL, PriorityString(PRIO_NORMAL))->Check(currentPriority == PRIO_NORMAL);
    prio->AppendSeparator();
    prio->AppendCheckItem(ptID_PRIO_SKIP, PriorityString(PRIO_SKIP))->Check(currentPriority == PRIO_SKIP);

    wxMenu menu;
    menu.AppendSubMenu(prio, "Priority");
    menu.AppendSeparator();
    menu.Append(ptID_RENAME_FILE, "Rename file")->Enable(!isMulti);

    PopupMenu(&menu);
}

void AddTorrentDialog::OnMenu(wxCommandEvent& event)
{
    lt::sha1_hash hash = items_[torrentsCombo_->GetSelection()];
    lt::add_torrent_params& p = params_[hash];

    long idx = -1;

    while ((idx = fileList_->GetNextItem(idx, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != wxNOT_FOUND)
    {
        switch (event.GetId())
        {
        case ptID_PRIO_MAX:
            p.file_priorities[idx] = PRIO_MAX;
            break;

        case ptID_PRIO_HIGH:
            p.file_priorities[idx] = PRIO_HIGH;
            break;

        case ptID_PRIO_NORMAL:
            p.file_priorities[idx] = PRIO_NORMAL;
            break;

        case ptID_PRIO_SKIP:
            p.file_priorities[idx] = PRIO_SKIP;
            break;

        case ptID_RENAME_FILE:
        {
            wxTextEntryDialog dlg(this,
                "New file name",
                "Rename file",
                p.ti->files().file_name(idx));

            if (dlg.ShowModal() == wxID_OK)
            {
                std::string newName = dlg.GetValue().ToStdString();
                p.ti->rename_file(idx, newName);
                fileList_->SetItem(idx, 0, newName);
            }
        }
        break;
        }

        fileList_->SetItem(idx, 2, PriorityString(p.file_priorities[idx]));
    }
}

void AddTorrentDialog::OnSavePathChanged(wxCommandEvent& WXUNUSED(event))
{
    // Do not do anything if we select the "All torrents" option.
    if (params_.size() > 1 && torrentsCombo_->GetSelection() == 0)
    {
        return;
    }

    lt::sha1_hash hash = items_[torrentsCombo_->GetSelection()];
    lt::add_torrent_params& p = params_[hash];
    p.save_path = savePathText_->GetValue().ToStdString();
}

std::string AddTorrentDialog::PriorityString(int priority)
{
    switch (priority)
    {
    case PRIO_SKIP:
        return "Do not download";
    case PRIO_NORMAL:
        return "Normal";
    case PRIO_HIGH:
        return "High";
    case PRIO_MAX:
        return "Maximum";
    }

    return "<unknown>";
}
