#include "persistenttorrentlistview.hpp"

#include "torrentlistview.hpp"
#include "torrentlistviewmodel.hpp"

#include "scaler.hpp"

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
        m_tlv->GetColumn(TorrentListViewModel::Columns::Name)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_QueuePosition_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::QueuePosition)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_Size_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Size)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_Status_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Status)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_Progress_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Progress)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_ETA_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::ETA)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_DownloadSpeed_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::DownloadSpeed)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_UploadSpeed_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::UploadSpeed)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_Ratio_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Ratio)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_Seeds_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Seeds)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_Peers_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::Peers)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_AddedOn_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::AddedOn)->SetWidth(SX(colWidth));
    }

    if (RestoreValue("Col_CompletedOn_Width", &colWidth))
    {
        m_tlv->GetColumn(TorrentListViewModel::Columns::CompletedOn)->SetWidth(SX(colWidth));
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
    // before storing values, remove dpi scaling
    SaveValue("Col_Name_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::Name)->GetWidth()));
    SaveValue("Col_QueuePosition_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::QueuePosition)->GetWidth()));
    SaveValue("Col_Size_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::Size)->GetWidth()));
    SaveValue("Col_Status_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::Status)->GetWidth()));
    SaveValue("Col_Progress_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::Progress)->GetWidth()));
    SaveValue("Col_ETA_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::ETA)->GetWidth()));
    SaveValue("Col_DownloadSpeed_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::DownloadSpeed)->GetWidth()));
    SaveValue("Col_UploadSpeed_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::UploadSpeed)->GetWidth()));
    SaveValue("Col_Ratio_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::Ratio)->GetWidth()));
    SaveValue("Col_Seeds_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::Seeds)->GetWidth()));
    SaveValue("Col_Peers_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::Peers)->GetWidth()));
    SaveValue("Col_AddedOn_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::AddedOn)->GetWidth()));
    SaveValue("Col_CompletedOn_Width", iSX(m_tlv->GetColumn(TorrentListViewModel::Columns::CompletedOn)->GetWidth()));
}

PersistentTorrentListView* pt::wxCreatePersistentObject(TorrentListView* lv)
{
    return new PersistentTorrentListView(lv);
}
