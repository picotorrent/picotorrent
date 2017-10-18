#include "addtorrentdlg.hpp"

#include "translator.hpp"
#include "utils.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/filepicker.h>
#include <wx/treelist.h>

#include <shellapi.h>

namespace lt = libtorrent;
using pt::AddTorrentDialog;

wxBEGIN_EVENT_TABLE(AddTorrentDialog, wxDialog)
	EVT_CHOICE(ptID_TORRENT_LIST, OnTorrentChanged)
	EVT_DIRPICKER_CHANGED(ptID_SAVE_PATH, OnSavePathChanged)
	EVT_TREELIST_ITEM_CONTEXT_MENU(ptID_TORRENT_FILE_LIST, OnTorrentFileContextMenu)
wxEND_EVENT_TABLE()

AddTorrentDialog::AddTorrentDialog(wxWindow* parent,
	std::shared_ptr<pt::Translator> translator,
	std::vector<lt::add_torrent_params>& params)
	: wxDialog(parent, wxID_ANY, "Add torrent(s)", wxDefaultPosition, wxSize(400, 400)),
	m_params(params),
	m_icons(new wxImageList(16, 16)),
	m_trans(translator)
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	m_torrents = new wxChoice(panel, ptID_TORRENT_LIST);
	m_size = new wxStaticText(panel, wxID_ANY, wxEmptyString);
	m_savePath = new wxDirPickerCtrl(panel, ptID_SAVE_PATH, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);

	wxFlexGridSizer* flexGrid = new wxFlexGridSizer(3, 2, 9, 25);
	flexGrid->AddGrowableCol(1, 1);

	// Torrent
	flexGrid->Add(new wxStaticText(panel, wxID_ANY, m_trans->Translate("torrent")));

	for (lt::add_torrent_params& p : m_params)
	{
		m_torrents->Insert(p.ti->name(), 0);
	}

	flexGrid->Add(m_torrents, 1, wxEXPAND);

	// Size
	flexGrid->Add(new wxStaticText(panel, wxID_ANY, m_trans->Translate("size")));
	flexGrid->Add(m_size);

	// Save path
	flexGrid->Add(new wxStaticText(panel, wxID_ANY, m_trans->Translate("save_path")));
	flexGrid->Add(m_savePath, 1, wxEXPAND);

	SHSTOCKICONINFO inf = { 0 };
	inf.cbSize = sizeof(SHSTOCKICONINFO);

	if (SHGetStockIconInfo(SIID_FOLDER, SHGSI_ICON | SHGSI_SMALLICON, &inf) == S_OK)
	{
		wxIcon icon;
		icon.CreateFromHICON(static_cast<WXHICON>(inf.hIcon));
		m_icons->Add(icon);
	}

	if (SHGetStockIconInfo(SIID_FOLDEROPEN, SHGSI_ICON | SHGSI_SMALLICON, &inf) == S_OK)
	{
		wxIcon icon;
		icon.CreateFromHICON(static_cast<WXHICON>(inf.hIcon));
		m_icons->Add(icon);
	}

	// File tree
	m_torrentFiles = new wxTreeListCtrl(panel, ptID_TORRENT_FILE_LIST);
	m_torrentFiles->SetImageList(m_icons);
	m_torrentFiles->AppendColumn("Name", 100);
	m_torrentFiles->AppendColumn("Size", 80);
	m_torrentFiles->AppendColumn("Priority", 80);

	wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonsSizer->Add(new wxButton(panel, wxID_OK));
	buttonsSizer->Add(new wxButton(panel, wxID_CANCEL));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(flexGrid, 0, wxALL | wxEXPAND, 5);
	mainSizer->Add(m_torrentFiles, 1, wxALL | wxEXPAND, 5);
	mainSizer->Add(buttonsSizer, 0, wxALL | wxALIGN_RIGHT, 5);

	panel->SetSizerAndFit(mainSizer);

	m_torrents->Select(0);
	LoadTorrentInfo(0);
}

AddTorrentDialog::~AddTorrentDialog()
{
	delete m_icons;
}

void AddTorrentDialog::LoadTorrentInfo(int index)
{
	lt::add_torrent_params& params = m_params.at(index);

	// Size
	m_size->SetLabel(wxString(Utils::ToHumanFileSize(params.ti->total_size())));

	// Save path
	m_savePath->SetPath(params.save_path);

	// Files
	m_torrentFiles->DeleteAllItems();
	lt::file_storage const& files = params.ti->files();

	for (int i = 0; i < files.num_files(); i++)
	{
		lt::file_index_t idx(i);

		m_torrentFiles->AppendItem(
			m_torrentFiles->GetRootItem(),
			files.file_path(idx));
	}
}

void AddTorrentDialog::OnSavePathChanged(wxFileDirPickerEvent& event)
{
	int idx = m_torrents->GetSelection();
	lt::add_torrent_params& params = m_params.at(idx);
	params.save_path = event.GetPath().ToStdString();
}

void AddTorrentDialog::OnTorrentChanged(wxCommandEvent& event)
{
	LoadTorrentInfo(event.GetInt());
}

void AddTorrentDialog::OnTorrentFileContextMenu(wxTreeListEvent& event)
{
	wxMenu* prioMenu = new wxMenu();
	prioMenu->Append(wxID_ANY, m_trans->Translate("priority"));
	prioMenu->Append(wxID_ANY, m_trans->Translate("high"));
	prioMenu->Append(wxID_ANY, m_trans->Translate("normal"));
	prioMenu->AppendSeparator();
	prioMenu->Append(wxID_ANY, m_trans->Translate("do_not_download"));

	wxMenu menu;
	menu.AppendSubMenu(prioMenu, m_trans->Translate("priority"));

	PopupMenu(&menu);
}
