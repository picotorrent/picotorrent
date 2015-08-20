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
#include "../controllers/addtorrentcontroller.h"

wxBEGIN_EVENT_TABLE(AddTorrentDialog, wxFrame)
    EVT_BUTTON(wxID_SAVEAS, AddTorrentDialog::OnBrowseSavePath)
    EVT_BUTTON(wxID_ADD, AddTorrentDialog::OnAdd)
    EVT_COMBOBOX(ptID_TORRENTS_COMBO, AddTorrentDialog::OnComboSelected)
    EVT_LIST_ITEM_RIGHT_CLICK(ptID_TORRENT_FILES_LIST, AddTorrentDialog::OnFileItemRightClick)
    EVT_MENU(wxID_ANY, AddTorrentDialog::OnMenu)
    EVT_TEXT(ptID_SAVE_PATH, AddTorrentDialog::OnSavePathChanged)
wxEND_EVENT_TABLE()

AddTorrentDialog::AddTorrentDialog(wxWindow* parent,
    boost::shared_ptr<AddTorrentController> controller)
    : wxFrame(parent, wxID_ANY, wxT("Add torrent(s)"), wxDefaultPosition, wxSize(400, 485), wxCAPTION | wxCLOSE_BOX),
    controller_(controller)
{
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

    for (int i = 0; i < controller_->GetCount(); i++)
    {
        torrentsCombo_->Insert(controller_->GetName(i), torrentsCombo_->GetCount());
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
    fileList_->DeleteAllItems();

    for (int i = 0; i < controller_->GetFileCount(index); i++)
    {
        int prio = controller_->GetFilePriority(index, i);

        int idx = fileList_->InsertItem(fileList_->GetItemCount(), controller_->GetFileName(index, i));
        fileList_->SetItem(idx, 1, controller_->GetFileSize(index, i));
        fileList_->SetItem(idx, 2, controller_->ToFriendlyPriority(prio));
    }

    // Update torrent information
    sizeText_->SetLabel(controller_->GetSize(index));
    commentText_->SetLabel(controller_->GetComment(index));
    dateText_->SetLabel(controller_->GetCreationDate(index));
    creatorText_->SetLabel(controller_->GetCreator(index));
    savePathText_->SetLabel(controller_->GetSavePath(index));
}

void AddTorrentDialog::OnBrowseSavePath(wxCommandEvent& WXUNUSED(event))
{
    int idx = torrentsCombo_->GetSelection();
    std::string initialPath = controller_->GetSavePath(idx);

    wxDirDialog dlg(this, wxT("Browse"), initialPath);

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    controller_->SetSavePath(idx, dlg.GetPath().ToStdString());
    savePathText_->SetLabel(dlg.GetPath());
}

void AddTorrentDialog::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    controller_->PerformAdd();
    Destroy();
}

void AddTorrentDialog::OnFileItemRightClick(wxListEvent& event)
{
    int torrentIndex = torrentsCombo_->GetSelection();
    int fileIndex = (int)event.GetIndex();
    bool isMulti = (fileList_->GetSelectedItemCount() > 1);

    int currentPriority = controller_->GetFilePriority(torrentIndex, fileIndex);
    std::string friendlyPriority = controller_->ToFriendlyPriority(currentPriority);

    wxMenu* prio = new wxMenu();
    prio->AppendCheckItem(ptID_PRIO_MAX, controller_->ToFriendlyPriority(PRIO_MAX))->Check(currentPriority == PRIO_MAX);
    prio->AppendCheckItem(ptID_PRIO_HIGH, controller_->ToFriendlyPriority(PRIO_HIGH))->Check(currentPriority == PRIO_HIGH);
    prio->AppendCheckItem(ptID_PRIO_NORMAL, controller_->ToFriendlyPriority(PRIO_NORMAL))->Check(currentPriority == PRIO_NORMAL);
    prio->AppendSeparator();
    prio->AppendCheckItem(ptID_PRIO_SKIP, controller_->ToFriendlyPriority(PRIO_SKIP))->Check(currentPriority == PRIO_SKIP);

    wxMenu menu;
    menu.AppendSubMenu(prio, "Priority");
    menu.AppendSeparator();
    menu.Append(ptID_RENAME_FILE, "Rename file")->Enable(!isMulti);

    PopupMenu(&menu);
}

void AddTorrentDialog::OnMenu(wxCommandEvent& event)
{
    int torrentIndex = torrentsCombo_->GetSelection();
    long idx = -1;

    while ((idx = fileList_->GetNextItem(idx, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != wxNOT_FOUND)
    {
        switch (event.GetId())
        {
        case ptID_PRIO_MAX:
            controller_->SetFilePriority(torrentIndex, idx, PRIO_MAX);
            break;

        case ptID_PRIO_HIGH:
            controller_->SetFilePriority(torrentIndex, idx, PRIO_HIGH);
            break;

        case ptID_PRIO_NORMAL:
            controller_->SetFilePriority(torrentIndex, idx, PRIO_NORMAL);
            break;

        case ptID_PRIO_SKIP:
            controller_->SetFilePriority(torrentIndex, idx, PRIO_SKIP);
            break;

        case ptID_RENAME_FILE:
        {
            wxTextEntryDialog dlg(this,
                "New file name",
                "Rename file",
                controller_->GetFileName(torrentIndex, idx));

            if (dlg.ShowModal() == wxID_OK)
            {
                std::string newName = dlg.GetValue().ToStdString();
                controller_->SetFileName(torrentIndex, idx, newName);
                fileList_->SetItem(idx, 0, newName);
            }
        }
        break;
        }

        fileList_->SetItem(idx, 2, controller_->ToFriendlyPriority(controller_->GetFilePriority(torrentIndex, idx)));
    }
}

void AddTorrentDialog::OnSavePathChanged(wxCommandEvent& WXUNUSED(event))
{
    int torrentIndex = torrentsCombo_->GetSelection();
    controller_->SetSavePath(torrentIndex, savePathText_->GetValue().ToStdString());
}
