#include "addtorrentdialog.h"

#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dirdlg.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/sizer.h>
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
    : wxFrame(parent, wxID_ANY, wxT("Add torrent(s)"), wxDefaultPosition, wxSize(400, 500)),
    controller_(controller)
{
    panel_ = new wxPanel(this);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* torrentSizer = new wxStaticBoxSizer(wxVERTICAL,
        panel_,
        wxT("Torrents"));

    /* Set up the "Torrents" box */
    wxFlexGridSizer* torrentGrid = new wxFlexGridSizer(5, 2, 10, 25);
    torrentGrid->AddGrowableCol(1, 1);

    torrentGrid->Add(new wxStaticText(panel_, wxID_ANY, wxT("Name")), 1, wxALIGN_CENTER_VERTICAL);
    torrentsCombo_ = new wxComboBox(panel_,
        ptID_TORRENTS_COMBO,
        wxEmptyString,
        wxPoint(-1, -1),
        wxSize(-1, -1),
        wxArrayString(),
        wxCB_READONLY);
    torrentGrid->Add(torrentsCombo_, 1, wxEXPAND | wxLEFT | wxRIGHT);

    for (int i = 0; i < controller_->GetCount(); i++)
    {
        torrentsCombo_->Insert(controller_->GetName(i), torrentsCombo_->GetCount());
    }

    // Size
    torrentGrid->Add(new wxStaticText(panel_, wxID_ANY, wxT("Size")));
    sizeText_ = new wxStaticText(panel_, wxID_ANY, wxT("-"));
    torrentGrid->Add(sizeText_, 1, wxEXPAND);

    // Comment
    torrentGrid->Add(new wxStaticText(panel_, wxID_ANY, wxT("Comment")));
    commentText_ = new wxStaticText(panel_, wxID_ANY, wxT("-"));
    torrentGrid->Add(commentText_, 1, wxEXPAND);

    // Creation date
    torrentGrid->Add(new wxStaticText(panel_, wxID_ANY, wxT("Creation date")));
    dateText_ = new wxStaticText(panel_, wxID_ANY, wxT("-"));
    torrentGrid->Add(dateText_, 1, wxEXPAND);

    // Creator
    torrentGrid->Add(new wxStaticText(panel_, wxID_ANY, wxT("Creator")));
    creatorText_ = new wxStaticText(panel_, wxID_ANY, wxT("-"));
    torrentGrid->Add(creatorText_, 1, wxEXPAND);

    torrentSizer->Add(torrentGrid, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    sizer->Add(torrentSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);

    /* Set up the "Storage" box */
    wxStaticBoxSizer* storageSizer = new wxStaticBoxSizer(wxVERTICAL,
        panel_,
        wxT("Storage"));

    wxBoxSizer* storageRows = new wxBoxSizer(wxVERTICAL);

    // Save path
    wxBoxSizer* pathSizer = new wxBoxSizer(wxHORIZONTAL);
    savePathText_ = new wxTextCtrl(panel_, ptID_SAVE_PATH);
    pathSizer->Add(savePathText_, 1);
    pathSizer->Add(new wxButton(panel_, wxID_SAVEAS, wxT("Browse")), 0);

    storageRows->Add(pathSizer, 0, wxEXPAND | wxTOP | wxBOTTOM, 5);

    // files
    fileList_ = new wxListCtrl(panel_,
        ptID_TORRENT_FILES_LIST,
        wxDefaultPosition,
        wxSize(370, 190),
        wxLC_REPORT);
    storageRows->Add(fileList_, 1, wxEXPAND | wxALL);

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

    storageSizer->Add(storageRows, 1, wxEXPAND | wxALL, 5);
    sizer->Add(storageSizer, 1, wxEXPAND | wxALL, 5);

    /* Button! */
    sizer->Add(new wxButton(panel_, wxID_ADD, wxT("Add torrent(s)")), 0, wxALIGN_RIGHT | wxALL, 5);

    panel_->SetSizerAndFit(sizer);

    torrentsCombo_->Select(0);
    ShowTorrentInfo(0);
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
