#include "torrentlistview.hpp"

#include "translator.hpp"

using pt::TorrentListView;

TorrentListView::TorrentListView(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
	: wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE)
{
	AppendTextColumn(i18n(tr, "name"), 0, wxDATAVIEW_CELL_INERT, 180);
	AppendTextColumn(i18n(tr, "queue_position"), 1, wxDATAVIEW_CELL_INERT, 30, wxALIGN_RIGHT);
	AppendTextColumn(i18n(tr, "size"), 2, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn(i18n(tr, "status"), 3, wxDATAVIEW_CELL_INERT, 120);
	AppendProgressColumn(i18n(tr, "progress"), 4, wxDATAVIEW_CELL_INERT, 100);
	AppendTextColumn(i18n(tr, "eta"), 5, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn(i18n(tr, "dl"), 6, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn(i18n(tr, "ul"), 7, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn(i18n(tr, "ratio"), 8, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn(i18n(tr, "seeds"), 9, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
	AppendTextColumn(i18n(tr, "peers"), 10, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
}

wxSize TorrentListView::GetMinSize() const
{
	return wxSize(500, 150);
}
