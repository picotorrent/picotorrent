#include "addtorrentdlg.hpp"

#include "filecontextmenu.hpp"
#include "filestorageviewmodel.hpp"
#include "translator.hpp"
#include "utils.hpp"

#include "scaler.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/dataview.h>
#include <wx/filepicker.h>
#include <wx/persist.h>
#include <wx/persist/toplevel.h>

#include <shellapi.h>


namespace lt = libtorrent;
using pt::AddTorrentDialog;

wxBEGIN_EVENT_TABLE(AddTorrentDialog, wxDialog)
    EVT_CHECKBOX(ptID_SEQUENTIAL_DOWNLOAD, OnSequentialDownloadChanged)
    EVT_CHECKBOX(ptID_START_TORRENT, OnStartTorrentChanged)
    EVT_CHOICE(ptID_TORRENT_LIST, OnTorrentChanged)
    EVT_DATAVIEW_ITEM_CONTEXT_MENU(ptID_FILE_LIST, OnFileContextMenu)
    EVT_DIRPICKER_CHANGED(ptID_SAVE_PATH, OnSavePathChanged)
    EVT_MENU(pt::FileContextMenu::ptID_PRIO_MAXIMUM, OnSetPriority)
    EVT_MENU(pt::FileContextMenu::ptID_PRIO_LOW, OnSetPriority)
    EVT_MENU(pt::FileContextMenu::ptID_PRIO_NORMAL, OnSetPriority)
    EVT_MENU(pt::FileContextMenu::ptID_PRIO_DO_NOT_DOWNLOAD, OnSetPriority)
wxEND_EVENT_TABLE()

AddTorrentDialog::AddTorrentDialog(wxWindow* parent,
    std::shared_ptr<pt::Translator> translator,
    std::vector<lt::add_torrent_params>& params)
    : wxDialog(parent, wxID_ANY, i18n(translator, "add_torrent_s"), wxDefaultPosition, wxSize(SX(400), SY(540)), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    m_params(params),
    m_trans(translator),
    m_filesViewModel(new FileStorageViewModel(translator))
{
    this->SetName("AddTorrentDialog");
    this->SetMinSize(this->GetSize());

    wxPanel* pnl = new wxPanel(this, wxID_ANY);

    // File
    wxStaticBoxSizer* fileSizer = new wxStaticBoxSizer(wxVERTICAL, pnl, i18n(translator, "file"));
    m_torrents = new wxChoice(fileSizer->GetStaticBox(), ptID_TORRENT_LIST);
    fileSizer->Add(m_torrents, 0, wxEXPAND | wxALL, 5);

    // Torrent
    wxStaticBoxSizer* torrentSizer = new wxStaticBoxSizer(wxVERTICAL, pnl, i18n(translator, "torrent"));
    wxFlexGridSizer* torrentGrid = new wxFlexGridSizer(2, 10, 25);

    m_name = new wxStaticText(torrentSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
    m_size = new wxStaticText(torrentSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
    m_comment = new wxStaticText(torrentSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
    m_infoHash = new wxStaticText(torrentSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);

    torrentGrid->AddGrowableCol(1, 1);
    torrentGrid->Add(new wxStaticText(torrentSizer->GetStaticBox(), wxID_ANY, m_trans->Translate("name")));
    torrentGrid->Add(m_name, 0, wxEXPAND);
    torrentGrid->Add(new wxStaticText(torrentSizer->GetStaticBox(), wxID_ANY, m_trans->Translate("size")));
    torrentGrid->Add(m_size, 0, wxEXPAND);
    torrentGrid->Add(new wxStaticText(torrentSizer->GetStaticBox(), wxID_ANY, m_trans->Translate("info_hash")));
    torrentGrid->Add(m_infoHash, 0, wxEXPAND);
    torrentGrid->Add(new wxStaticText(torrentSizer->GetStaticBox(), wxID_ANY, m_trans->Translate("comment")));
    torrentGrid->Add(m_comment, 0, wxEXPAND);
    torrentSizer->Add(torrentGrid, 1, wxEXPAND | wxALL, 5);

    // Storage
    wxStaticBoxSizer* storageSizer = new wxStaticBoxSizer(wxVERTICAL, pnl, i18n(translator, "preferences"));
    wxFlexGridSizer* storageGrid = new wxFlexGridSizer(2, 10, 25);

    m_savePath = new wxDirPickerCtrl(storageSizer->GetStaticBox(), ptID_SAVE_PATH, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);
    m_filesView = new wxDataViewCtrl(storageSizer->GetStaticBox(), ptID_FILE_LIST, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE);
    m_sequentialMode = new wxCheckBox(storageSizer->GetStaticBox(), ptID_SEQUENTIAL_DOWNLOAD, i18n(m_trans, "sequential_download"));
    m_startTorrent = new wxCheckBox(storageSizer->GetStaticBox(), ptID_START_TORRENT, i18n(m_trans, "start_torrent"));
    m_startTorrent->SetValue(true);

    auto nameCol = m_filesView->AppendIconTextColumn(
        i18n(m_trans, "name"),
        FileStorageViewModel::Columns::Name,
        wxDATAVIEW_CELL_INERT,
        SX(220),
        wxALIGN_LEFT);

    m_filesView->AppendTextColumn(
        i18n(m_trans, "size"),
        FileStorageViewModel::Columns::Size,
        wxDATAVIEW_CELL_INERT,
        SX(80),
        wxALIGN_RIGHT);

    auto prioCol = m_filesView->AppendTextColumn(
        i18n(m_trans, "priority"),
        FileStorageViewModel::Columns::Priority,
        wxDATAVIEW_CELL_INERT,
        SX(80),
        wxALIGN_RIGHT);

    nameCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
    prioCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);

    m_filesView->AssociateModel(m_filesViewModel);
    m_filesViewModel->DecRef();

    wxBoxSizer* checkSizer = new wxBoxSizer(wxHORIZONTAL);
    checkSizer->Add(m_sequentialMode, 1, wxEXPAND);
    checkSizer->Add(m_startTorrent, 1, wxEXPAND);

    storageGrid->AddGrowableCol(1, 1);
    storageGrid->Add(new wxStaticText(storageSizer->GetStaticBox(), wxID_ANY, m_trans->Translate("save_path")), 0, wxALIGN_CENTER_VERTICAL);
    storageGrid->Add(m_savePath, 1, wxEXPAND);
    storageGrid->AddSpacer(0);
    storageGrid->Add(checkSizer, 1, wxEXPAND);

    storageSizer->Add(storageGrid, 0, wxEXPAND | wxALL, 5);
    storageSizer->Add(m_filesView, 1, wxEXPAND | wxALL, 5);

    // Buttons
    wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* ok = new wxButton(pnl, wxID_OK);
    ok->SetDefault();

    buttonsSizer->Add(ok);
    buttonsSizer->Add(new wxButton(pnl, wxID_CANCEL));

    // Main sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(fileSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->AddSpacer(2);
    mainSizer->Add(torrentSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->AddSpacer(2);
    mainSizer->Add(storageSizer, 1, wxEXPAND | wxALL, 5);
    mainSizer->AddSpacer(2);
    mainSizer->Add(buttonsSizer, 0, wxALL | wxALIGN_RIGHT, 5);

    for (lt::add_torrent_params& p : m_params)
    {
        wxString name = "<unknown name>";

        if (p.ti)
        {
            name = wxString::FromUTF8(p.ti->name());
        }
        else if (!p.name.empty())
        {
            name = wxString::FromUTF8(p.name);
        }
        else
        {
            std::stringstream ss;
            ss << p.info_hash;
            name = ss.str();
        }

        m_torrents->Insert(name, 0);
    }

    pnl->SetSizer(mainSizer);

    wxPersistenceManager::Get().RegisterAndRestore(this);

    m_torrents->Select(0);
    LoadTorrentInfo(0);
}

void AddTorrentDialog::LoadTorrentInfo(int index)
{
    lt::add_torrent_params& params = m_params.at(index);

    m_name->SetLabel("-");
    m_size->SetLabel("-");
    m_comment->SetLabel("-");
    m_infoHash->SetLabel("-");

    // Save path
    m_savePath->SetPath(wxString::FromUTF8(params.save_path));
    m_sequentialMode->SetValue(
        (params.flags & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download);

    if (params.ti)
    {
        m_name->SetLabel(wxString::FromUTF8(params.ti->name()));

        std::stringstream ss;
        ss << params.ti->info_hash();

        m_infoHash->SetLabel(ss.str());

        // Size
        m_size->SetLabel(wxString(Utils::ToHumanFileSize(params.ti->total_size())));

        if (!params.ti->comment().empty())
        {
            m_comment->SetLabel(wxString::FromUTF8(params.ti->comment()));
        }

        // Files
        m_filesViewModel->RebuildTree(params.ti);
        m_filesViewModel->UpdatePriorities(params.file_priorities);
        m_filesView->Expand(m_filesViewModel->GetRootItem());
    }
    else
    {
        if (!params.info_hash.is_all_zeros())
        {
            std::stringstream ss;
            ss << params.info_hash;

            m_infoHash->SetLabel(ss.str());
        }

        m_size->SetLabel("-");
        m_filesViewModel->Cleared();
    }
}

void AddTorrentDialog::OnSavePathChanged(wxFileDirPickerEvent& event)
{
    int idx = m_torrents->GetSelection();
    lt::add_torrent_params& params = m_params.at(idx);
    params.save_path = event.GetPath().ToUTF8();
}

void AddTorrentDialog::OnSetPriority(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_filesView->GetSelections(items);

    for (wxDataViewItem& item : items)
    {
        std::vector<int> fileIndices = m_filesViewModel->GetFileIndices(item);

        int idx = m_torrents->GetSelection();
        lt::add_torrent_params& params = m_params.at(idx);

        for (auto index : fileIndices)
        {
            switch (event.GetId())
            {
            case FileContextMenu::ptID_PRIO_MAXIMUM:
                params.file_priorities.at(index) = lt::top_priority;
                break;
            case FileContextMenu::ptID_PRIO_NORMAL:
                params.file_priorities.at(index) = lt::default_priority;
                break;
            case FileContextMenu::ptID_PRIO_LOW:
                params.file_priorities.at(index) = lt::low_priority;
                break;
            case FileContextMenu::ptID_PRIO_DO_NOT_DOWNLOAD:
                params.file_priorities.at(index) = lt::dont_download;
                break;
            }
        }

        m_filesViewModel->UpdatePriorities(params.file_priorities);
    }
}

void AddTorrentDialog::OnSequentialDownloadChanged(wxCommandEvent&)
{
    int idx = m_torrents->GetSelection();
    lt::add_torrent_params& params = m_params.at(idx);

    if (m_sequentialMode->IsChecked())
    {
        params.flags |= lt::torrent_flags::sequential_download;
    }
    else
    {
        params.flags &= ~lt::torrent_flags::sequential_download;
    }
}

void AddTorrentDialog::OnStartTorrentChanged(wxCommandEvent&)
{
    int idx = m_torrents->GetSelection();
    lt::add_torrent_params& params = m_params.at(idx);

    if (m_startTorrent->IsChecked())
    {
        params.flags &= ~lt::torrent_flags::paused;
        params.flags |= lt::torrent_flags::auto_managed;
    }
    else
    {
        params.flags |= lt::torrent_flags::paused;
        params.flags &= ~lt::torrent_flags::auto_managed;
    }
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
