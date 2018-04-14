#include "torrentlistview.hpp"

#include "torrentlistviewmodel.hpp"
#include "translator.hpp"

using pt::TorrentListView;

TorrentListView::TorrentListView(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
    : wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE)
{
    auto nameCol = AppendTextColumn(
        i18n(tr, "name"),
        TorrentListViewModel::Columns::Name,
        wxDATAVIEW_CELL_INERT,
        180,
        wxALIGN_NOT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "queue_position"),
        TorrentListViewModel::Columns::QueuePosition,
        wxDATAVIEW_CELL_INERT,
        30,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "size"),
        TorrentListViewModel::Columns::Size,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    auto statusCol = AppendTextColumn(
        i18n(tr, "status"),
        TorrentListViewModel::Columns::Status,
        wxDATAVIEW_CELL_INERT,
        120);

    AppendProgressColumn(
        i18n(tr, "progress"),
        TorrentListViewModel::Columns::Progress,
        wxDATAVIEW_CELL_INERT,
        100,
        wxALIGN_CENTER,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "eta"),
        TorrentListViewModel::Columns::ETA,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "dl"),
        TorrentListViewModel::Columns::DownloadSpeed,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "ul"),
        TorrentListViewModel::Columns::UploadSpeed,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "availability"),
        TorrentListViewModel::Columns::Availability,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "ratio"),
        TorrentListViewModel::Columns::Ratio,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "seeds"),
        TorrentListViewModel::Columns::Seeds,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT);

    AppendTextColumn(
        i18n(tr, "peers"),
        TorrentListViewModel::Columns::Peers,
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_RIGHT);

    AppendTextColumn(
        i18n(tr, "added_on"),
        TorrentListViewModel::Columns::AddedOn,
        wxDATAVIEW_CELL_INERT,
        120,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    AppendTextColumn(
        i18n(tr, "completed_on"),
        TorrentListViewModel::Columns::CompletedOn,
        wxDATAVIEW_CELL_INERT,
        120,
        wxALIGN_RIGHT,
        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    // Ugly hack to prevent the last "real" column from stretching.
    AppendColumn(new wxDataViewColumn(wxEmptyString, new wxDataViewTextRenderer(), -1, 0));

    nameCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
    statusCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
}

wxSize TorrentListView::GetMinSize() const
{
    return wxSize(500, 150);
}

void TorrentListView::Sort()
{
    if (GetSortingColumn())
    {
        auto sortingColumn = GetSortingColumn();
        auto model = reinterpret_cast<TorrentListViewModel*>(GetModel());

        if (HasSelection())
        {
            std::vector<lt::sha1_hash> hashes;

            wxDataViewItemArray items;
            GetSelections(items);

            for (auto item : items)
            {
                int row = model->GetRow(item);
                hashes.push_back(model->FindHashByRow(row));
            }

            model->Sort(
                GetColumnIndex(sortingColumn),
                sortingColumn->IsSortOrderAscending());

            for (auto hash : hashes)
            {
                int sortedRowIndex = model->GetRowIndex(hash);
                Select(model->GetItem(sortedRowIndex));
            }
        }
        else
        {
            model->Sort(
                GetColumnIndex(sortingColumn),
                sortingColumn->IsSortOrderAscending());
        }
    }
}
