#include "addtorrentdialog.hpp"

#include <regex>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/bmpcbox.h>
#include <wx/dataview.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/splitter.h>

#include "../../bittorrent/addparams.hpp"
#include "../../bittorrent/session.hpp"
#include "../../core/configuration.hpp"
#include "../../core/database.hpp"
#include "../../core/utils.hpp"
#include "../clientdata.hpp"
#include "../models/filestoragemodel.hpp"
#include "../translator.hpp"

using pt::UI::Dialogs::AddTorrentDialog;

AddTorrentDialog::AddTorrentDialog(wxWindow* parent, wxWindowID id, lt::add_torrent_params& params, std::shared_ptr<Core::Database> db, std::shared_ptr<Core::Configuration> cfg, std::shared_ptr<BitTorrent::Session> session)
    : wxDialog(parent, id, GetTorrentDisplayName(params), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    m_params(params),
    m_db(db),
    m_cfg(cfg),
    m_session(session),
    m_filesModel(new Models::FileStorageModel(std::bind(&AddTorrentDialog::SetFilePriorities, this, std::placeholders::_1, std::placeholders::_2))),
    m_splitter(new wxSplitterWindow(this, wxID_ANY))
{
    m_splitter->SetWindowStyleFlag(
        m_splitter->GetWindowStyleFlag() | wxSP_LIVE_UPDATE | wxSP_BORDER);
    m_splitter->SetMinimumPaneSize(10);

    auto infoPanel = new wxPanel(m_splitter, wxID_ANY);
    auto filesPanel = new wxPanel(m_splitter, wxID_ANY);

    m_splitter->SplitVertically(
        infoPanel,
        filesPanel);

    // Left side panel
    auto savePathSizer = new wxStaticBoxSizer(wxHORIZONTAL, infoPanel, i18n("save_path"));
    m_torrentSavePath = new wxComboBox(savePathSizer->GetStaticBox(), ptID_SAVE_PATH_INPUT);
    m_torrentSavePathBrowse = new wxButton(savePathSizer->GetStaticBox(), ptID_SAVE_PATH_BROWSE, i18n("browse"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    savePathSizer->Add(m_torrentSavePath, 1, wxEXPAND | wxALL, FromDIP(3));
    savePathSizer->Add(m_torrentSavePathBrowse, 0, wxTOP | wxRIGHT | wxBOTTOM, FromDIP(3));

    auto optionsSizer = new wxStaticBoxSizer(wxVERTICAL, infoPanel, i18n("options"));
    m_torrentLabel = new wxBitmapComboBox(optionsSizer->GetStaticBox(), ptID_LABEL_COMBO, "", wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
    m_sequentialDownload = new wxCheckBox(optionsSizer->GetStaticBox(), ptID_SEQUENTIAL_DOWNLOAD, i18n("sequential_download"));
    m_startTorrent = new wxCheckBox(optionsSizer->GetStaticBox(), ptID_START_TORRENT, i18n("start_torrent"));

    auto optionsGrid = new wxFlexGridSizer(2, FromDIP(7), FromDIP(10));
    optionsGrid->AddGrowableCol(1, 1);
    optionsGrid->Add(new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, i18n("label")), 0, wxALIGN_CENTER_VERTICAL);
    optionsGrid->Add(m_torrentLabel, 1, wxEXPAND | wxALL, FromDIP(3));

    auto flagsGrid = new wxFlexGridSizer(2, FromDIP(7), FromDIP(10));
    flagsGrid->Add(m_sequentialDownload, 1, wxALL);
    flagsGrid->Add(m_startTorrent, 1, wxALL);

    optionsGrid->AddSpacer(1);
    optionsGrid->Add(flagsGrid, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, FromDIP(3));

    optionsSizer->Add(optionsGrid, 1, wxEXPAND);

    // tabs with collections
    auto nb = new wxNotebook(infoPanel, wxID_ANY);

    m_trackers = new wxListView(nb, wxID_ANY);
    m_trackers->AppendColumn(i18n("url"), wxLIST_FORMAT_LEFT, FromDIP(180));

    nb->AddPage(m_trackers, i18n("trackers"), true);
    nb->AddPage(new wxPanel(nb, wxID_ANY), i18n("peers"));

    auto infoSizer = new wxBoxSizer(wxVERTICAL);
    infoSizer->Add(savePathSizer, 0, wxEXPAND | wxTOP | wxLEFT | wxBOTTOM, FromDIP(11));
    infoSizer->Add(optionsSizer, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(11));
    infoSizer->Add(nb, 1, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(11));
    infoPanel->SetSizerAndFit(infoSizer);

    auto contentsSizer = new wxStaticBoxSizer(wxVERTICAL, filesPanel, i18n("contents"));
    m_filesView = new wxDataViewCtrl(contentsSizer->GetStaticBox(), ptID_FILE_LIST, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE);
    m_torrentName = new wxStaticText(contentsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
    m_torrentSize = new wxStaticText(contentsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_torrentInfoHash = new wxStaticText(contentsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_torrentComment = new wxStaticText(contentsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);

    auto infoGrid = new wxFlexGridSizer(2, FromDIP(3), FromDIP(15));
    infoGrid->AddGrowableCol(1, 1);
    infoGrid->Add(new wxStaticText(contentsSizer->GetStaticBox(), wxID_ANY, i18n("name")), 0, wxEXPAND);
    infoGrid->Add(m_torrentName);
    infoGrid->Add(new wxStaticText(contentsSizer->GetStaticBox(), wxID_ANY, i18n("size")));
    infoGrid->Add(m_torrentSize);
    infoGrid->Add(new wxStaticText(contentsSizer->GetStaticBox(), wxID_ANY, i18n("info_hash")));
    infoGrid->Add(m_torrentInfoHash);
    infoGrid->Add(new wxStaticText(contentsSizer->GetStaticBox(), wxID_ANY, i18n("comment")));
    infoGrid->Add(m_torrentComment);
    contentsSizer->Add(infoGrid, 0, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, FromDIP(3));
    contentsSizer->Add(m_filesView, 1, wxEXPAND | wxALL, FromDIP(3));

    auto filesSizer = new wxBoxSizer(wxVERTICAL);
    filesSizer->Add(contentsSizer, 1, wxEXPAND | wxTOP | wxRIGHT | wxBOTTOM, FromDIP(11));
    filesPanel->SetSizerAndFit(filesSizer);

    auto nameCol = new wxDataViewColumn(
        i18n("name"),
        new wxDataViewCheckIconTextRenderer(),
        Models::FileStorageModel::Columns::Name,
        FromDIP(180),
        wxALIGN_LEFT);

    m_filesView->AppendColumn(nameCol);

    m_filesView->AppendTextColumn(
        i18n("size"),
        Models::FileStorageModel::Columns::Size,
        wxDATAVIEW_CELL_INERT,
        FromDIP(80),
        wxALIGN_RIGHT);

    auto prioCol = m_filesView->AppendTextColumn(
        i18n("priority"),
        Models::FileStorageModel::Columns::Priority,
        wxDATAVIEW_CELL_INERT,
        FromDIP(80));

    // Ugly hack to prevent the last "real" column from stretching.
    m_filesView->AppendColumn(new wxDataViewColumn(wxEmptyString, new wxDataViewTextRenderer(), Models::FileStorageModel::Columns::_Max, 0));

    nameCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
    prioCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);

    m_filesView->AssociateModel(m_filesModel);
    m_filesModel->DecRef();

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok = new wxButton(this, wxID_OK, i18n("ok"));
    ok->SetDefault();

    buttonsSizer->Add(ok);
    buttonsSizer->AddSpacer(FromDIP(7));
    buttonsSizer->Add(new wxButton(this, wxID_CANCEL, i18n("cancel")));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_splitter, 1, wxEXPAND, 0);
    sizer->Add(buttonsSizer, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(11));
    sizer->SetSizeHints(this);

    this->SetSizerAndFit(sizer);
    this->SetSize(FromDIP(wxSize(750, 450)));

    // Load labels
    Core::Configuration::Label lbl;
    lbl.id = -1;
    lbl.savePath = m_cfg->Get<std::string>("default_save_path").value();
    lbl.savePathEnabled = true;

    m_torrentLabel->Append(i18n("none"), wxNullBitmap, new ClientData<Core::Configuration::Label>(lbl));

    for (auto const& label : cfg->GetLabels())
    {
        wxBitmap bmp = wxNullBitmap;

        if (label.colorEnabled && label.color.size() > 0)
        {
            bmp = wxBitmap(24, 24);

            {
                wxMemoryDC dc;
                wxDCBrushChanger dcbc(dc, wxColor(label.color));
                dc.SelectObject(bmp);
                dc.Clear();
            }
        }

        m_torrentLabel->Append(
            Utils::toStdWString(label.name),
            bmp,
            new ClientData<Core::Configuration::Label>(label));
    }

    // Load save path history
    auto stmt = m_db->CreateStatement("SELECT path FROM path_history WHERE type = 'add_torrent_dialog' ORDER BY timestamp DESC LIMIT 5");

    while (stmt->Read())
    {
        m_torrentSavePath->Insert(
            Utils::toStdWString(stmt->GetString(0)),
            m_torrentSavePath->GetCount());
    }

    // Load trackers
    for (auto const& tracker : m_params.trackers)
    {
        int row = m_trackers->GetItemCount();
        m_trackers->InsertItem(row, tracker);
    }

    this->Bind(
        wxEVT_COMBOBOX,
        [this](wxCommandEvent&)
        {
            auto label = reinterpret_cast<ClientData<Core::Configuration::Label>*>(
                m_torrentLabel->GetClientObject(
                    m_torrentLabel->GetSelection()));

            if (m_manualSavePath.find(m_params.info_hashes) == m_manualSavePath.end()
                && label->GetValue().savePathEnabled
                && !label->GetValue().savePath.empty())
            {
                m_params.save_path = label->GetValue().savePath;
                m_torrentSavePath->ChangeValue(Utils::toStdWString(m_params.save_path));
            }

            m_params.userdata.get<BitTorrent::AddParams>()->labelId = label->GetValue().id;
        },
        ptID_LABEL_COMBO);

    this->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            wxDirDialog dlg(
                m_parent,
                wxDirSelectorPromptStr,
                wxEmptyString,
                wxDD_DIR_MUST_EXIST);

            if (dlg.ShowModal() != wxID_OK)
            {
                return;
            }

            m_torrentSavePath->SetValue(dlg.GetPath());
        },
        ptID_SAVE_PATH_BROWSE);

    this->Bind(
        wxEVT_TEXT,
        [this](wxCommandEvent&)
        {
            m_params.save_path = Utils::toStdString(m_torrentSavePath->GetValue().wc_str());
            m_manualSavePath.insert(m_params.info_hashes);
        },
        ptID_SAVE_PATH_INPUT);

    this->Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent&)
        {
            if (m_sequentialDownload->IsChecked()) { m_params.flags |= lt::torrent_flags::sequential_download; }
            else { m_params.flags &= ~lt::torrent_flags::sequential_download; }
        },
        ptID_SEQUENTIAL_DOWNLOAD);

    this->Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent&)
        {
            if (m_startTorrent->IsChecked())
            {
                m_params.flags |= lt::torrent_flags::auto_managed;
                m_params.flags &= ~lt::torrent_flags::paused;
            }
            else
            {
                m_params.flags &= ~lt::torrent_flags::auto_managed;
                m_params.flags |= lt::torrent_flags::paused;
            }
        },
        ptID_START_TORRENT);

    this->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &AddTorrentDialog::ShowFileContextMenu, this, ptID_FILE_LIST);
    this->Bind(ptEVT_TORRENT_METADATA_FOUND, [this](BitTorrent::MetadataFoundEvent& evt) { this->MetadataFound(evt.GetData()); });

    this->Load();
}

AddTorrentDialog::~AddTorrentDialog()
{
    {
        auto stmt = m_db->CreateStatement("INSERT INTO path_history (path, type, timestamp) VALUES(?, 'add_torrent_dialog', strftime('%s'))\n"
            "ON CONFLICT (path, type) DO UPDATE SET timestamp = excluded.timestamp;");
        stmt->Bind(1, m_params.save_path);
        stmt->Execute();
    }

    // Remove all entries except the last 5
    m_db->Execute("DELETE FROM path_history WHERE id NOT IN (SELECT id FROM path_history WHERE type = 'add_torrent_dialog' ORDER BY timestamp DESC LIMIT 5)");
}

void AddTorrentDialog::MetadataFound(std::shared_ptr<lt::torrent_info> const& ti)
{
    if (m_params.info_hashes == ti->info_hashes()
        || (ti->info_hashes().has_v1() && ti->info_hashes().v1 == m_params.info_hashes.v1)
        || (ti->info_hashes().has_v2() && ti->info_hashes().v2 == m_params.info_hashes.v2))
    {
        m_params.ti = ti;
        // TODO: refresh UI
    }
}

wxString AddTorrentDialog::GetTorrentDisplayName(libtorrent::add_torrent_params const& params)
{
    if (params.ti)
    {
        return Utils::toStdWString(params.ti->name());
    }

    if (params.name.size() > 0)
    {
        return Utils::toStdWString(params.name);
    }

    std::stringstream hash;

    if (params.info_hashes.has_v2()) hash << params.info_hashes.v2;
    if (params.info_hashes.has_v1()) hash << params.info_hashes.v1;

    return hash.str();
}

wxString AddTorrentDialog::GetTorrentDisplaySize(libtorrent::add_torrent_params const& params)
{
    if (params.ti)
    {
        return Utils::toHumanFileSize(params.ti->total_size());
    }

    return "-";
}

wxString AddTorrentDialog::GetTorrentDisplayInfoHash(libtorrent::add_torrent_params const& params)
{
    std::stringstream hash;

    if (params.ti)
    {
        if (params.ti->info_hashes().has_v2())
        {
            hash << params.ti->info_hashes().v2;
        }
        else
        {
            hash << params.ti->info_hashes().v1;
        }
    }
    else if (params.info_hashes.has_v1() || params.info_hashes.has_v2())
    {
        if (params.info_hashes.has_v2())
        {
            hash << params.info_hashes.v2;
        }
        else
        {
            hash << params.info_hashes.v1;
        }
    }
    else
    {
        hash << "-";
    }

    return hash.str();
}

wxString AddTorrentDialog::GetTorrentDisplayComment(libtorrent::add_torrent_params const& params)
{
    if (params.ti)
    {
        return params.ti->comment();
    }

    return "-";
}

void AddTorrentDialog::Load()
{
    m_torrentName->SetLabel(this->GetTorrentDisplayName(m_params));
    m_torrentSize->SetLabel(this->GetTorrentDisplaySize(m_params));
    m_torrentInfoHash->SetLabel(this->GetTorrentDisplayInfoHash(m_params));
    m_torrentComment->SetLabel(this->GetTorrentDisplayComment(m_params));

    // Save path
    m_torrentSavePath->ChangeValue(wxString::FromUTF8(m_params.save_path));

    m_sequentialDownload->SetValue(
        (m_params.flags & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download);

    bool isPaused = (m_params.flags & lt::torrent_flags::paused) == lt::torrent_flags::paused
        && (m_params.flags & lt::torrent_flags::auto_managed) != lt::torrent_flags::auto_managed;

    m_startTorrent->SetValue(!isPaused);

    if (m_params.ti)
    {
        // Files
        m_filesModel->RebuildTree(m_params.ti);
        m_filesModel->UpdatePriorities(m_params.file_priorities);

        wxDataViewItemArray children;
        m_filesModel->GetChildren(m_filesModel->GetRootItem(), children);
        for (auto const& child : children) { m_filesView->Expand(child); }
    }
    else
    {
        m_filesModel->Cleared();
    }

    // set correct label item
    m_torrentLabel->SetSelection(0);
    for (uint32_t i = 0; i < m_torrentLabel->GetCount(); i++)
    {
        auto label = reinterpret_cast<ClientData<Core::Configuration::Label>*>(m_torrentLabel->GetClientObject(i));

        if (label->GetValue().id == m_params.userdata.get<BitTorrent::AddParams>()->labelId)
        {
            // yup
            m_torrentLabel->SetSelection(i);
            break;
        }
    }
}

void AddTorrentDialog::SetFilePriorities(wxDataViewItemArray& items, lt::download_priority_t prio)
{
    auto fileIndices = m_filesModel->GetFileIndices(items);

    for (lt::file_index_t idx : fileIndices)
    {
        size_t fileIdx = static_cast<size_t>(int32_t(idx));

        if (m_params.file_priorities.size() <= fileIdx)
        {
            m_params.file_priorities.resize(fileIdx + 1, lt::default_priority);
        }

        m_params.file_priorities.at(fileIdx) = prio;
    }
}

void AddTorrentDialog::ShowFileContextMenu(wxDataViewEvent&)
{
    wxDataViewItemArray items;
    m_filesView->GetSelections(items);

    if (items.IsEmpty())
    {
        return;
    }

    auto fileIndices = m_filesModel->GetFileIndices(items);
    auto firstPrio = m_params.file_priorities.size() > 0
        ? m_params.file_priorities[static_cast<int>(fileIndices[0])]
        : lt::default_priority;

    auto allSamePrio = std::all_of(
        fileIndices.begin(),
        fileIndices.end(),
        [&](lt::file_index_t i)
        {
            size_t fileIdx = static_cast<size_t>(int32_t(i));
            auto p = m_params.file_priorities.size() >= fileIdx + 1
                ? m_params.file_priorities[fileIdx]
                : lt::default_priority;
            return firstPrio == p;
        });

    wxMenu* prioMenu = new wxMenu();
    prioMenu->AppendCheckItem(ptID_CONTEXT_MENU_MAXIMUM, i18n("maximum"))
        ->Check(allSamePrio && firstPrio == lt::top_priority);
    prioMenu->AppendCheckItem(ptID_CONTEXT_MENU_NORMAL, i18n("normal"))
        ->Check(allSamePrio && firstPrio == lt::default_priority);
    prioMenu->AppendCheckItem(ptID_CONTEXT_MENU_LOW, i18n("low"))
        ->Check(allSamePrio && firstPrio == lt::low_priority);
    prioMenu->AppendSeparator();
    prioMenu->AppendCheckItem(ptID_CONTEXT_MENU_DO_NOT_DOWNLOAD, i18n("do_not_download"))
        ->Check(allSamePrio && firstPrio == lt::dont_download);

    wxMenu menu;
    menu.AppendSubMenu(prioMenu, i18n("priority"));
    menu.Bind(
        wxEVT_MENU,
        [this, &items](wxCommandEvent& evt)
        {
            switch (evt.GetId())
            {
            case ptID_CONTEXT_MENU_DO_NOT_DOWNLOAD:
                SetFilePriorities(items, lt::dont_download);
                break;
            case ptID_CONTEXT_MENU_LOW:
                SetFilePriorities(items, lt::low_priority);
                break;
            case ptID_CONTEXT_MENU_MAXIMUM:
                SetFilePriorities(items, lt::top_priority);
                break;
            case ptID_CONTEXT_MENU_NORMAL:
                SetFilePriorities(items, lt::default_priority);
                break;
            }
        });

    PopupMenu(&menu);

    m_filesModel->UpdatePriorities(m_params.file_priorities);
}
