#include "torrentfilelistview.hpp"

#include "models/filestoragemodel.hpp"
#include "translator.hpp"

using pt::UI::TorrentFileListView;
using pt::UI::Models::FileStorageModel;

TorrentFileListView::TorrentFileListView(wxWindow* parent, wxWindowID id)
    : wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE, wxDefaultValidator, "TorrentFileListView")
{
    auto nameCol = AppendIconTextColumn(
        i18n("name"),
        FileStorageModel::Columns::Name,
        wxDATAVIEW_CELL_INERT,
        FromDIP(220),
        wxALIGN_LEFT);

    AppendTextColumn(
        i18n("size"),
        FileStorageModel::Columns::Size,
        wxDATAVIEW_CELL_INERT,
        FromDIP(80),
        wxALIGN_RIGHT);

    AppendProgressColumn(
        i18n("progress"),
        FileStorageModel::Columns::Progress,
        wxDATAVIEW_CELL_INERT,
        FromDIP(80));

    AppendTextColumn(
        i18n("priority"),
        FileStorageModel::Columns::Priority,
        wxDATAVIEW_CELL_INERT,
        FromDIP(120));

    // Ugly hack to prevent the last "real" column from stretching.
    AppendColumn(new wxDataViewColumn(wxEmptyString, new wxDataViewTextRenderer(), FileStorageModel::Columns::_Max, 0));

    nameCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
}

TorrentFileListView::~TorrentFileListView()
{
}
