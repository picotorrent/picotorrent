#include "torrentlistview.hpp"

#include <libtorrent/torrent_status.hpp>
#include <wx/dataview.h>

namespace lt = libtorrent;
using pt::TorrentListView;

class TorrentListView::Model : public wxDataViewVirtualListModel
{
public:
	std::vector<lt::torrent_status> torrents;

	Model() : wxDataViewVirtualListModel()
	{
	}

	virtual unsigned int GetColumnCount() const
	{
		return 0;
	}

	virtual wxString GetColumnType(unsigned int col) const
	{
		return "string";
	}

	virtual void GetValueByRow(wxVariant &variant, unsigned row, unsigned col) const
	{
		lt::torrent_status const& ts = torrents.at(row);

		switch (col)
		{
		case 0:
		{
			variant = wxVariant(ts.name);
			break;
		}
		case 1:
		{
			variant = wxVariant(ts.queue_position + 1);
			break;
		}
		case 2:
		{
			variant = wxVariant(static_cast<long>(54));
			break;
		}
		}
	}

	virtual bool SetValueByRow(const wxVariant &variant, unsigned row, unsigned col)
	{
		return false;
	}
};

TorrentListView::TorrentListView(wxWindow* parent)
	: wxPanel(parent)
{
	m_torrentsModel = new Model();

	m_torrentsView = new wxDataViewCtrl(this, wxID_ANY);
	m_torrentsView->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT, 180);
	m_torrentsView->AppendTextColumn("#", 1, wxDATAVIEW_CELL_INERT, 30, wxALIGN_RIGHT);
	m_torrentsView->AppendProgressColumn("Progress", 2);
	m_torrentsView->AssociateModel(m_torrentsModel);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_torrentsView, 1, wxEXPAND, 0);

	this->SetSizerAndFit(sizer);
}

void TorrentListView::AddTorrent(lt::torrent_status const& torrent)
{
	m_torrentsModel->torrents.push_back(torrent);
	m_torrentsModel->RowAppended();
}

wxSize TorrentListView::GetMinSize() const
{
	return wxSize(400, 100);
}
