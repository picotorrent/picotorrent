#include "persistenttorrentlistview.hpp"

#include "torrentlistview.hpp"
#include "torrentlistviewmodel.hpp"

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
