#include "addtorrentdlg.hpp"

#include "filecontextmenu.hpp"
#include "filestorageviewmodel.hpp"
#include "translator.hpp"
#include "utils.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/dataview.h>
#include <wx/filepicker.h>

#include <shellapi.h>

namespace lt = libtorrent;
using pt::AddTorrentDialog;

wxBEGIN_EVENT_TABLE(AddTorrentDialog, wxDialog)
    EVT_CHOICE(ptID_TORRENT_LIST, OnTorrentChanged)
    EVT_DATAVIEW_ITEM_CONTEXT_MENU(ptID_FILE_LIST, OnFileContextMenu)
    EVT_DIRPICKER_CHANGED(ptID_SAVE_PATH, OnSavePathChanged)
    EVT_MENU(pt::FileContextMenu::ptID_PRIO_MAXIMUM, OnSetPriority)
    EVT_MENU(pt::FileContextMenu::ptID_PRIO_HIGH, OnSetPriority)
    EVT_MENU(pt::FileContextMenu::ptID_PRIO_NORMAL, OnSetPriority)
    EVT_MENU(pt::FileContextMenu::ptID_PRIO_SKIP, OnSetPriority)
wxEND_EVENT_TABLE()

AddTorrentDialog::AddTorrentDialog(wxWindow* parent,
    std::shared_ptr<pt::Translator> translator,
    std::vector<lt::add_torrent_params>& params)
    : wxDialog(parent, wxID_ANY, i18n(translator, "add_torrent_s"), wxDefaultPosition, wxSize(400, 400)),
    m_params(params),
    m_trans(translator),
    m_filesViewModel(new FileStorageViewModel(translator))
{
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    m_torrents = new wxChoice(panel, ptID_TORRENT_LIST);
    m_size = new wxStaticText(panel, wxID_ANY, wxEmptyString);
    m_savePath = new wxDirPickerCtrl(panel, ptID_SAVE_PATH, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);
    m_filesView = new wxDataViewCtrl(panel, ptID_FILE_LIST);

    wxFlexGridSizer* flexGrid = new wxFlexGridSizer(3, 2, 9, 25);
    flexGrid->AddGrowableCol(1, 1);

    // Torrent
    flexGrid->Add(new wxStaticText(panel, wxID_ANY, m_trans->Translate("torrent")), 0, wxALIGN_CENTER_VERTICAL);

    for (lt::add_torrent_params& p : m_params)
    {
        wxString name = "<unknown name>";

        if (p.ti)
        {
            name = p.ti->name();
        }
        else
        {
            std::stringstream ss;
            ss << p.info_hash;
            name = ss.str();
        }

        m_torrents->Insert(name, 0);
    }

    flexGrid->Add(m_torrents, 1, wxEXPAND);

    // Size
    flexGrid->Add(new wxStaticText(panel, wxID_ANY, m_trans->Translate("size")), 0, wxALIGN_CENTER_VERTICAL);
    flexGrid->Add(m_size);

    // Save path
    flexGrid->Add(new wxStaticText(panel, wxID_ANY, m_trans->Translate("save_path")), 0, wxALIGN_CENTER_VERTICAL);
    flexGrid->Add(m_savePath, 1, wxEXPAND);

    // File tree
    auto nameCol = m_filesView->AppendIconTextColumn(
        i18n(m_trans, "name"),
        FileStorageViewModel::Columns::Name,
        wxDATAVIEW_CELL_INERT,
        220,
        wxALIGN_LEFT);

    m_filesView->AppendTextColumn(
        i18n(m_trans, "size"),
        FileStorageViewModel::Columns::Size,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT);

    auto prioCol = m_filesView->AppendTextColumn(
        i18n(m_trans, "priority"),
        FileStorageViewModel::Columns::Priority,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT);

    nameCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
    prioCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);

    m_filesView->AssociateModel(m_filesViewModel);

    wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(new wxButton(panel, wxID_OK));
    buttonsSizer->Add(new wxButton(panel, wxID_CANCEL));

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(flexGrid, 0, wxALL | wxEXPAND, 5);
    mainSizer->Add(m_filesView, 1, wxALL | wxEXPAND, 5);
    mainSizer->Add(buttonsSizer, 0, wxALL | wxALIGN_RIGHT, 5);

    panel->SetSizerAndFit(mainSizer);

    m_torrents->Select(0);
    LoadTorrentInfo(0);
}

void AddTorrentDialog::LoadTorrentInfo(int index)
{
    lt::add_torrent_params& params = m_params.at(index);

    // Save path
    m_savePath->SetPath(params.save_path);

    if (params.ti)
    {
        // Size
        m_size->SetLabel(wxString(Utils::ToHumanFileSize(params.ti->total_size())));

        // Files
        m_filesViewModel->RebuildTree(params.ti);
        m_filesViewModel->UpdatePriorities(params.file_priorities);
        m_filesView->Expand(m_filesViewModel->GetRootItem());
    }
    else
    {
        m_size->SetLabel("-");
        m_filesViewModel->Cleared();
    }
}

void AddTorrentDialog::OnSavePathChanged(wxFileDirPickerEvent& event)
{
    int idx = m_torrents->GetSelection();
    lt::add_torrent_params& params = m_params.at(idx);
    params.save_path = event.GetPath().ToStdString();
}

void AddTorrentDialog::OnSetPriority(wxCommandEvent& event)
{
    wxDataViewItem item = m_filesView->GetSelection();
    std::vector<int> fileIndices = m_filesViewModel->GetFileIndices(item);

    int idx = m_torrents->GetSelection();
    lt::add_torrent_params& params = m_params.at(idx);

    for (auto index : fileIndices)
    {
        switch (event.GetId())
        {
        case FileContextMenu::ptID_PRIO_MAXIMUM:
            params.file_priorities.at(index) = 7;
            break;
        case FileContextMenu::ptID_PRIO_HIGH:
            params.file_priorities.at(index) = 6;
            break;
        case FileContextMenu::ptID_PRIO_NORMAL:
            params.file_priorities.at(index) = 4;
            break;
        case FileContextMenu::ptID_PRIO_SKIP:
            params.file_priorities.at(index) = 0;
            break;
        }
    }

    m_filesViewModel->UpdatePriorities(params.file_priorities);
}

void AddTorrentDialog::OnTorrentChanged(wxCommandEvent& event)
{
    LoadTorrentInfo(event.GetInt());
}

void AddTorrentDialog::OnFileContextMenu(wxDataViewEvent& event)
{
    FileContextMenu menu(m_trans);
    PopupMenu(&menu);
}
