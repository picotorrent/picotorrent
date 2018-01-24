#include "torrentlistview.hpp"

#include "torrentlistviewmodel.hpp"
#include "translator.hpp"

using pt::PersistentTorrentListView;
using pt::TorrentListView;

PersistentTorrentListView::PersistentTorrentListView(TorrentListView* tlv)
    : wxPersistentWindow<TorrentListView>(tlv),
    m_tlv(tlv)
{
}

wxString PersistentTorrentListView::GetKind() const
{
    return "torrentViewCtrl";
}

bool PersistentTorrentListView::Restore()
{
    int sortIndex;
    bool sortAscending;
    int colWidth;

    if (RestoreValue("Col_Name_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Name)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_QueuePosition_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::QueuePosition)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_Size_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Size)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_Status_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Status)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_Progress_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Progress)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_ETA_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::ETA)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_DownloadSpeed_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::DownloadSpeed)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_UploadSpeed_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::UploadSpeed)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_Ratio_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Ratio)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_Seeds_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Seeds)->SetWidth(colWidth);
    }

    if (RestoreValue("Col_Peers_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Peers)->SetWidth(colWidth);
    }

    if (RestoreValue("SortIndex", &sortIndex)
        && RestoreValue("SortAscending", &sortAscending))
    {
        m_tlv->GetColumn(sortIndex)->SetSortOrder(sortAscending);
        m_tlv->Sort();
    }

    return true;
}

void PersistentTorrentListView::Save() const
{
    if (m_tlv->GetSortingColumn() != nullptr)
    {
        int sortIndex = m_tlv->GetColumnIndex(m_tlv->GetSortingColumn());
        SaveValue("SortIndex", sortIndex);
        SaveValue("SortAscending", m_tlv->GetSortingColumn()->IsSortOrderAscending());
    }

    SaveValue("Col_Name_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::Name)->GetWidth());
    SaveValue("Col_QueuePosition_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::QueuePosition)->GetWidth());
    SaveValue("Col_Size_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::Size)->GetWidth());
    SaveValue("Col_Status_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::Status)->GetWidth());
    SaveValue("Col_Progress_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::Progress)->GetWidth());
    SaveValue("Col_ETA_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::ETA)->GetWidth());
    SaveValue("Col_DownloadSpeed_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::DownloadSpeed)->GetWidth());
    SaveValue("Col_UploadSpeed_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::UploadSpeed)->GetWidth());
    SaveValue("Col_Ratio_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::Ratio)->GetWidth());
    SaveValue("Col_Seeds_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::Seeds)->GetWidth());
    SaveValue("Col_Peers_Width", m_tlv->GetColumn(TorrentListViewModel::Columns::Peers)->GetWidth());
}

PersistentTorrentListView* pt::wxCreatePersistentObject(TorrentListView* lv)
{
    return new PersistentTorrentListView(lv);
}

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
            wxDataViewItem selection = GetSelection();
            int row = model->GetRow(selection);
            lt::sha1_hash& selectedHash = model->FindHashByRow(row);

            model->Sort(
                GetColumnIndex(sortingColumn),
                sortingColumn->IsSortOrderAscending());

            int sortedRowIndex = model->GetRowIndex(selectedHash);
            Select(model->GetItem(sortedRowIndex));
        }
        else
        {
            model->Sort(
                GetColumnIndex(sortingColumn),
                sortingColumn->IsSortOrderAscending());
        }
    }
}
