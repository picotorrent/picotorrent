#include "addtorrentdialog.hpp"

#include <regex>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/bmpcbox.h>
#include <wx/dataview.h>
#include <wx/sizer.h>

#include "../../bittorrent/addparams.hpp"
#include "../../core/configuration.hpp"
#include "../../core/database.hpp"
#include "../../core/utils.hpp"
#include "../clientdata.hpp"
#include "../models/filestoragemodel.hpp"
#include "../translator.hpp"

using pt::UI::Dialogs::AddTorrentDialog;

AddTorrentDialog::AddTorrentDialog(wxWindow* parent, wxWindowID id, std::vector<lt::add_torrent_params>& params, std::shared_ptr<Core::Database> db, std::shared_ptr<Core::Configuration> cfg)
    : wxDialog(parent, id, i18n("add_torrent_s"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    m_params(params),
    m_db(db),
    m_cfg(cfg),
    m_filesModel(new Models::FileStorageModel(std::bind(&AddTorrentDialog::SetFilePriorities, this, std::placeholders::_1, std::placeholders::_2)))
{
    auto fileSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("file"));

    m_torrents = new wxChoice(fileSizer->GetStaticBox(), ptID_TORRENTS_COMBO);
    fileSizer->Add(m_torrents, 0, wxEXPAND | wxALL, FromDIP(3));

    auto infoSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("torrent"));

    m_torrentName = new wxStaticText(infoSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
    m_torrentSize = new wxStaticText(infoSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_torrentInfoHash = new wxStaticText(infoSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_torrentComment = new wxStaticText(infoSizer->GetStaticBox(), wxID_ANY, wxEmptyString);

    auto infoGrid = new wxFlexGridSizer(2, FromDIP(7), FromDIP(25));
    infoGrid->AddGrowableCol(1, 1);
    infoGrid->Add(new wxStaticText(infoSizer->GetStaticBox(), wxID_ANY, i18n("name")));
    infoGrid->Add(m_torrentName);
    infoGrid->Add(new wxStaticText(infoSizer->GetStaticBox(), wxID_ANY, i18n("size")));
    infoGrid->Add(m_torrentSize);
    infoGrid->Add(new wxStaticText(infoSizer->GetStaticBox(), wxID_ANY, i18n("info_hash")));
    infoGrid->Add(m_torrentInfoHash);
    infoGrid->Add(new wxStaticText(infoSizer->GetStaticBox(), wxID_ANY, i18n("comment")));
    infoGrid->Add(m_torrentComment);
    infoSizer->Add(infoGrid);

    auto optionsSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("options"));

    m_torrentLabel = new wxBitmapComboBox(optionsSizer->GetStaticBox(), ptID_LABEL_COMBO, "", wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
    m_torrentSavePath = new wxComboBox(optionsSizer->GetStaticBox(), ptID_SAVE_PATH_INPUT);
    m_torrentSavePathBrowse = new wxButton(optionsSizer->GetStaticBox(), ptID_SAVE_PATH_BROWSE, i18n("browse"));
    m_filesView = new wxDataViewCtrl(optionsSizer->GetStaticBox(), ptID_FILE_LIST, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE);
    m_sequentialDownload = new wxCheckBox(optionsSizer->GetStaticBox(), ptID_SEQUENTIAL_DOWNLOAD, i18n("sequential_download"));
    m_startTorrent = new wxCheckBox(optionsSizer->GetStaticBox(), ptID_START_TORRENT, i18n("start_torrent"));

    auto optionsGrid = new wxFlexGridSizer(2, FromDIP(7), FromDIP(10));
    optionsGrid->AddGrowableCol(1, 1);

    optionsGrid->Add(new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, i18n("label")), 0, wxALIGN_CENTER_VERTICAL);
    optionsGrid->Add(m_torrentLabel, 1, wxEXPAND | wxALL, FromDIP(3));

    auto savePathSizer = new wxBoxSizer(wxHORIZONTAL);
    savePathSizer->Add(m_torrentSavePath, 1, wxALL, FromDIP(3));
    savePathSizer->Add(m_torrentSavePathBrowse, 0, wxALL, FromDIP(3));

    optionsGrid->Add(new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, i18n("save_path")), 0, wxALIGN_CENTER_VERTICAL);
    optionsGrid->Add(savePathSizer, 1, wxEXPAND);

    // seq. download, paused, etc

    auto flagsGrid = new wxFlexGridSizer(2, FromDIP(7), FromDIP(10));
    flagsGrid->Add(m_sequentialDownload, 1, wxALL);
    flagsGrid->Add(m_startTorrent, 1, wxALL);

    optionsGrid->AddSpacer(1);
    optionsGrid->Add(flagsGrid, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, FromDIP(3));

    optionsSizer->Add(optionsGrid, 0, wxEXPAND);
    optionsSizer->Add(m_filesView, 1, wxEXPAND | wxALL, FromDIP(3));

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
    m_filesView->AppendColumn(new wxDataViewColumn(wxEmptyString, new wxDataViewTextRenderer(), -1, 0));

    nameCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
    prioCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);

    m_filesView->AssociateModel(m_filesModel);
    m_filesModel->DecRef();

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok = new wxButton(this, wxID_OK);
    ok->SetDefault();

    buttonsSizer->Add(ok);
    buttonsSizer->AddSpacer(FromDIP(7));
    buttonsSizer->Add(new wxButton(this, wxID_CANCEL));

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(fileSizer, 0, wxEXPAND | wxALL, FromDIP(11));
    mainSizer->Add(infoSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(11));
    mainSizer->Add(optionsSizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(11));
    mainSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(11));

    this->SetSizerAndFit(mainSizer);
    this->SetSize(FromDIP(wxSize(400, 500)));
    this->SetMinSize(FromDIP(wxSize(400, 450)));

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

    // Load torrents
    for (auto const& param : m_params)
    {
        m_torrents->Insert(
            this->GetTorrentDisplayName(param),
            m_torrents->GetCount());
    }

    m_torrents->Select(0);

    this->Bind(wxEVT_CHOICE, [this](wxCommandEvent& evt) { this->Load(evt.GetInt()); }, ptID_TORRENTS_COMBO);

    this->Bind(
        wxEVT_COMBOBOX,
        [this](wxCommandEvent&)
        {
            int idx = m_torrents->GetSelection();
            lt::add_torrent_params& params = m_params.at(idx);
            auto label = reinterpret_cast<ClientData<Core::Configuration::Label>*>(m_torrentLabel->GetClientObject(m_torrentLabel->GetSelection()));

            if (m_manualSavePath.find(params.info_hashes) == m_manualSavePath.end()
                && label->GetValue().savePathEnabled
                && !label->GetValue().savePath.empty())
            {
                params.save_path = label->GetValue().savePath;
                m_torrentSavePath->ChangeValue(Utils::toStdWString(params.save_path));
            }

            params.userdata.get<BitTorrent::AddParams>()->labelId = label->GetValue().id;
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
            int idx = m_torrents->GetSelection();
            lt::add_torrent_params& params = m_params.at(idx);
            params.save_path = Utils::toStdString(m_torrentSavePath->GetValue().wc_str());
            m_manualSavePath.insert(params.info_hashes);
        },
        ptID_SAVE_PATH_INPUT);

    this->Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent&)
        {
            int idx = m_torrents->GetSelection();
            lt::add_torrent_params& params = m_params.at(idx);
            if (m_sequentialDownload->IsChecked()) { params.flags |= lt::torrent_flags::sequential_download; }
            else { params.flags &= ~lt::torrent_flags::sequential_download; }
        },
        ptID_SEQUENTIAL_DOWNLOAD);

    this->Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent&)
        {
            int idx = m_torrents->GetSelection();
            lt::add_torrent_params& params = m_params.at(idx);
            if (m_startTorrent->IsChecked())
            {
                params.flags |= lt::torrent_flags::auto_managed;
                params.flags &= ~lt::torrent_flags::paused;
            }
            else
            {
                params.flags &= ~lt::torrent_flags::auto_managed;
                params.flags |= lt::torrent_flags::paused;
            }
        },
        ptID_START_TORRENT);

    this->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &AddTorrentDialog::ShowFileContextMenu, this, ptID_FILE_LIST);

    this->Load(0);
}

AddTorrentDialog::~AddTorrentDialog()
{
    for (int i = 0; i < m_params.size(); i++)
    {
        lt::add_torrent_params const& p = m_params.at(i);

        auto stmt = m_db->CreateStatement("INSERT INTO path_history (path, type, timestamp) VALUES(?, 'add_torrent_dialog', strftime('%s'))\n"
            "ON CONFLICT (path, type) DO UPDATE SET timestamp = excluded.timestamp;");
        stmt->Bind(1, p.save_path);
        stmt->Execute();
    }

    // Remove all entries except the last 5
    m_db->Execute("DELETE FROM path_history WHERE id NOT IN (SELECT id FROM path_history WHERE type = 'add_torrent_dialog' ORDER BY timestamp DESC LIMIT 5)");
}

void AddTorrentDialog::MetadataFound(std::shared_ptr<lt::torrent_info> const& ti)
{
    for (size_t i = 0; i < m_params.size(); i++)
    {
        auto& params = m_params.at(i);

        if (params.info_hashes == ti->info_hashes()
            || (ti->info_hashes().has_v1() && ti->info_hashes().v1 == params.info_hashes.v1)
            || (ti->info_hashes().has_v2() && ti->info_hashes().v2 == params.info_hashes.v2))
        {
            params.ti = ti;

            m_torrents->SetString(i, GetTorrentDisplayName(params));

            if (i == m_torrents->GetSelection())
            {
                Load(i);
            }
        }
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

void AddTorrentDialog::Load(size_t index)
{
    auto const& params = m_params.at(index);

    m_torrentName->SetLabel(this->GetTorrentDisplayName(params));
    m_torrentSize->SetLabel(this->GetTorrentDisplaySize(params));
    m_torrentInfoHash->SetLabel(this->GetTorrentDisplayInfoHash(params));
    m_torrentComment->SetLabel(this->GetTorrentDisplayComment(params));

    // Save path
    m_torrentSavePath->ChangeValue(wxString::FromUTF8(params.save_path));

    m_sequentialDownload->SetValue(
        (params.flags & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download);

    bool isPaused = (params.flags & lt::torrent_flags::paused) == lt::torrent_flags::paused
        && (params.flags & lt::torrent_flags::auto_managed) != lt::torrent_flags::auto_managed;

    m_startTorrent->SetValue(!isPaused);

    if (params.ti)
    {
        // Files
        m_filesModel->RebuildTree(params.ti);
        m_filesModel->UpdatePriorities(params.file_priorities);

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

        if (label->GetValue().id == params.userdata.get<BitTorrent::AddParams>()->labelId)
        {
            // yup
            m_torrentLabel->SetSelection(i);
            break;
        }
    }
}

void AddTorrentDialog::SetFilePriorities(wxDataViewItemArray& items, lt::download_priority_t prio)
{
    auto& param = m_params.at(m_torrents->GetSelection());
    auto fileIndices = m_filesModel->GetFileIndices(items);

    for (lt::file_index_t idx : fileIndices)
    {
        if (param.file_priorities.size() <= static_cast<int>(idx))
        {
            param.file_priorities.resize(static_cast<int>(idx) + 1, lt::default_priority);
        }

        param.file_priorities.at(static_cast<int>(idx)) = prio;
    }
}

void AddTorrentDialog::ShowFileContextMenu(wxDataViewEvent& evt)
{
    wxDataViewItemArray items;
    m_filesView->GetSelections(items);

    if (items.IsEmpty())
    {
        return;
    }

    auto& param = m_params.at(m_torrents->GetSelection());
    auto fileIndices = m_filesModel->GetFileIndices(items);
    auto firstPrio = param.file_priorities.size() > 0
        ? param.file_priorities[static_cast<int>(fileIndices[0])]
        : lt::default_priority;

    auto allSamePrio = std::all_of(
        fileIndices.begin(),
        fileIndices.end(),
        [&](lt::file_index_t i)
        {
            int i32 = static_cast<int>(i);
            auto p = param.file_priorities.size() >= i32 + 1
                ? param.file_priorities[i32]
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
        [this, &items, &param](wxCommandEvent& evt)
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

    m_filesModel->UpdatePriorities(param.file_priorities);
}
