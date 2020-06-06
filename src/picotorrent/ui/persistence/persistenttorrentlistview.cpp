#include "persistenttorrentlistview.hpp"

#include <map>

#include "../../picojson.hpp"
#include "../models/torrentlistmodel.hpp"
#include "../torrentlistview.hpp"

using pt::UI::TorrentListView;
using pt::UI::Models::TorrentListModel;
using pt::UI::Persistence::PersistentTorrentListView;

static std::map<TorrentListModel::Columns, std::string> ColumnNames =
{
    { TorrentListModel::Columns::Name,          "Col_Name" },
    { TorrentListModel::Columns::QueuePosition, "Col_QueuePosition" },
    { TorrentListModel::Columns::Size,          "Col_Size" },
    { TorrentListModel::Columns::SizeRemaining, "Col_SizeRemaining" },
    { TorrentListModel::Columns::Status,        "Col_Status" },
    { TorrentListModel::Columns::Progress,      "Col_Progress" },
    { TorrentListModel::Columns::ETA,           "Col_ETA" },
    { TorrentListModel::Columns::DownloadSpeed, "Col_DownloadSpeed" },
    { TorrentListModel::Columns::UploadSpeed,   "Col_UploadSpeed" },
    { TorrentListModel::Columns::Availability,  "Col_Availability" },
    { TorrentListModel::Columns::Ratio,         "Col_Ratio" },
    { TorrentListModel::Columns::Seeds,         "Col_Seeds" },
    { TorrentListModel::Columns::Peers,         "Col_Peers" },
    { TorrentListModel::Columns::AddedOn,       "Col_AddedOn" },
    { TorrentListModel::Columns::CompletedOn,   "Col_CompletedOn" },
};

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
    wxString hiddenJson;
    wxString modelsJson;
    wxString widthsJson;

    if (!RestoreValue("Hidden", &hiddenJson)
        || !RestoreValue("Models", &modelsJson)
        || !RestoreValue("Widths", &widthsJson))
    {
        return false;
    }

    picojson::value hiddenValue;
    picojson::value modelsValue;
    picojson::value widthsValue;

    if (!picojson::parse(hiddenValue, hiddenJson.ToStdString()).empty()
        || !picojson::parse(modelsValue, modelsJson.ToStdString()).empty()
        || !picojson::parse(widthsValue, widthsJson.ToStdString()).empty())
    {
        return false;
    }

    picojson::array hidden = hiddenValue.get<picojson::array>();
    picojson::array models = modelsValue.get<picojson::array>();
    picojson::array widths = widthsValue.get<picojson::array>();

    if (hidden.size() != m_tlv->GetColumnCount()
        || models.size() != m_tlv->GetColumnCount()
        || widths.size() != m_tlv->GetColumnCount())
    {
        return false;
    }

    std::vector<wxDataViewColumn*> cols;
    cols.resize(m_tlv->GetColumnCount());

    for (uint32_t i = m_tlv->GetColumnCount(); i > 0 ; i--)
    {
        auto col = m_tlv->GetColumnAt(i - 1);

        // Calculate its order based on the index in the models array

        int64_t modelIndex = static_cast<int64_t>(col->GetModelColumn());

        auto iter = std::find(models.begin(), models.end(), picojson::value(modelIndex));
        if (iter == models.end()) { return false; }
        auto dist = std::distance(models.begin(), iter);

        cols[dist] = col;

        m_tlv->DeleteColumn(col);
    }

    for (uint32_t i = 0; i < cols.size(); i++)
    {
        auto col = cols.at(i);

        m_tlv->AppendColumn(col);

        col->SetHidden(hidden[i].get<bool>());
        col->SetWidth(static_cast<int>(widths[i].get<int64_t>()));
    }

    int sortIndex;
    bool sortAscending;

    if (RestoreValue("SortIndex", &sortIndex)
        && RestoreValue("SortAscending", &sortAscending))
    {
        m_tlv->GetColumn(sortIndex)->SetSortOrder(sortAscending);
    }

    return true;
}

void PersistentTorrentListView::Save() const
{
    if (auto sortingColumn = m_tlv->GetSortingColumn())
    {
        SaveValue("SortIndex", m_tlv->GetColumnIndex(sortingColumn));
        SaveValue("SortAscending", sortingColumn->IsSortOrderAscending());
    }

    picojson::array hidden;
    picojson::array models;
    picojson::array widths;

    for (uint32_t i = 0; i < m_tlv->GetColumnCount(); i++)
    {
        auto col = m_tlv->GetColumnAt(i);

        hidden.push_back(picojson::value(col->IsHidden()));
        models.push_back(picojson::value(static_cast<int64_t>(col->GetModelColumn())));
        widths.push_back(picojson::value(static_cast<int64_t>(col->GetWidth())));
    }

    SaveValue("Hidden", picojson::value(hidden).serialize());
    SaveValue("Models", picojson::value(models).serialize());
    SaveValue("Widths", picojson::value(widths).serialize());
}

PersistentTorrentListView* wxCreatePersistentObject(TorrentListView* lv)
{
    return new PersistentTorrentListView(lv);
}