#include "torrentlistview.hpp"

#include <wx/dataview.h>

using pt::TorrentListView;

TorrentListView::TorrentListView(wxWindow* parent)
	: wxPanel(parent)
{
	wxBoxSizer *txt1sizer = new wxBoxSizer(wxVERTICAL);
	wxDataViewCtrl* torrents = new wxDataViewCtrl(this, wxID_ANY);
	wxDataViewTextRenderer* rend0 = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE);
	wxDataViewTextRenderer* rend1 = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE);
	torrents->AppendColumn(new wxDataViewColumn("Name", rend0, 0, 120, wxAlignment(wxALIGN_LEFT | wxALIGN_TOP), wxDATAVIEW_COL_RESIZABLE));
	torrents->AppendColumn(new wxDataViewColumn("#", rend1, 0, 30, wxAlignment(wxALIGN_LEFT | wxALIGN_TOP), wxDATAVIEW_COL_RESIZABLE));

	txt1sizer->Add(torrents, 1, wxEXPAND, 0);
	this->SetSizer(txt1sizer);
}

wxSize TorrentListView::GetMinSize() const
{
	return wxSize(400, 100);
}
