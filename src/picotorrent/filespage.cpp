#include "filespage.hpp"

#include "filestorageviewmodel.hpp"
#include "string.hpp"
#include "translator.hpp"

#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/dataview.h>

namespace lt = libtorrent;
using pt::FilesPage;

FilesPage::FilesPage(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tran)
    : wxPanel(parent, id),
	m_filesView(new wxDataViewCtrl(this, wxID_ANY))
{
	m_filesView->AppendTextColumn(i18n(tran, "name"), 0, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT);
	m_filesView->AppendTextColumn(i18n(tran, "size"), 1, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_filesView, 1, wxEXPAND);

	this->SetSizer(sizer);
}

void FilesPage::Update(lt::torrent_status const& ts)
{
	std::shared_ptr<const lt::torrent_info> tf = ts.torrent_file.lock();
	if (!tf) { return; }

	if (m_hash != ts.info_hash)
	{
		m_hash = ts.info_hash;
		m_filesView->AssociateModel(new FileStorageViewModel(tf));
	}
}
