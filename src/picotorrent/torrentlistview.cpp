#include "torrentlistview.hpp"

using pt::TorrentListView;

TorrentListView::TorrentListView(wxWindow* parent, wxWindowID id)
	: wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE)
{
	AppendTextColumn(_("Name"), 0, wxDATAVIEW_CELL_INERT, 180);
	AppendTextColumn("#", 1, wxDATAVIEW_CELL_INERT, 30, wxALIGN_RIGHT);
	AppendTextColumn("Size", 2, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn("Status", 3, wxDATAVIEW_CELL_INERT, 120);
	AppendProgressColumn("Progress", 4, wxDATAVIEW_CELL_INERT, 100);
	AppendTextColumn("ETA", 5, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn("DL", 6, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn("UL", 7, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn("Ratio", 8, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn("Seeds", 9, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn("Peers", 10, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
}

wxSize TorrentListView::GetMinSize() const
{
	return wxSize(500, 150);
}
