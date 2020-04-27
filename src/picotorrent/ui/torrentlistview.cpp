#include "torrentlistview.hpp"

#include <wx/persist.h>

#include "../bittorrent/torrenthandle.hpp"
#include "models/torrentlistmodel.hpp"
#include "persistence/persistenttorrentlistview.hpp"
#include "translator.hpp"

using pt::UI::TorrentListView;
using pt::UI::Models::TorrentListModel;

TorrentListView::TorrentListView(wxWindow* parent, wxWindowID id, Models::TorrentListModel* model)
    : wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE, wxDefaultValidator, "TorrentListView"),
    m_model(model)
{
    this->AssociateModel(m_model);
    m_model->DecRef();

    auto defaultFlags = wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE;

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("name"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::Name,
                FromDIP(180),
                wxALIGN_NOT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("queue_position"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::QueuePosition,
                FromDIP(30),
                wxALIGN_RIGHT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("size"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::Size,
                FromDIP(80),
                wxALIGN_RIGHT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("size_remaining"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::SizeRemaining,
                FromDIP(80),
                wxALIGN_RIGHT,
                defaultFlags),
            true));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("status"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::Status,
                FromDIP(120),
                wxALIGN_NOT,
                wxDATAVIEW_COL_REORDERABLE)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("progress"),
                new wxDataViewProgressRenderer(),
                TorrentListModel::Columns::Progress,
                FromDIP(100),
                wxALIGN_NOT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("eta"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::ETA,
                FromDIP(80),
                wxALIGN_RIGHT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("dl"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::DownloadSpeed,
                FromDIP(80),
                wxALIGN_RIGHT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("ul"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::UploadSpeed,
                FromDIP(80),
                wxALIGN_RIGHT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("availability"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::Availability,
                FromDIP(80),
                wxALIGN_RIGHT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("ratio"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::Ratio,
                FromDIP(80),
                wxALIGN_RIGHT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("seeds"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::Seeds,
                FromDIP(80),
                wxALIGN_RIGHT,
                wxDATAVIEW_COL_REORDERABLE)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("peers"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::Peers,
                FromDIP(80),
                wxALIGN_RIGHT,
                wxDATAVIEW_COL_REORDERABLE)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("added_on"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::AddedOn,
                FromDIP(120),
                wxALIGN_RIGHT,
                defaultFlags)));

    m_columns.push_back(
        ColumnMetadata(
            new wxDataViewColumn(
                i18n("completed_on"),
                new wxDataViewTextRenderer(),
                TorrentListModel::Columns::CompletedOn,
                FromDIP(120),
                wxALIGN_RIGHT,
                defaultFlags)));

    /*
    nameCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
    statusCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);*/

    // Just add all columns. The persistence manager will set the saved
    // widths, order and hidden attributes. Otherwise we use our defaults.
    for (auto const& cmd : m_columns)
    {
        InsertColumn(GetColumnCount(), cmd.column);
    }

    if (!wxPersistenceManager::Get().RegisterAndRestore(this))
    {
        for (auto const& cmd : m_columns)
        {
            cmd.column->SetHidden(cmd.hidden);
        }
    }

    this->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, &TorrentListView::ShowHeaderContextMenu, this);
}

TorrentListView::~TorrentListView()
{
}

wxSize TorrentListView::GetMinSize() const
{
    return FromDIP(wxSize(400, 100));
}

void TorrentListView::Sort()
{
    if (auto sortingColumn = GetSortingColumn())
    {
        if (HasSelection())
        {
            std::vector<BitTorrent::TorrentHandle*> torrents;

            wxDataViewItemArray items;
            GetSelections(items);

            for (auto item : items)
            {
                torrents.push_back(m_model->GetTorrentFromItem(item));
            }

            m_model->Sort(
                GetColumnIndex(sortingColumn),
                sortingColumn->IsSortOrderAscending());

            for (auto torrent : torrents)
            {
                int sortedRowIndex = m_model->GetRowIndex(torrent);
                Select(m_model->GetItem(sortedRowIndex));
            }
        }
        else
        {
            m_model->Sort(
                GetColumnIndex(sortingColumn),
                sortingColumn->IsSortOrderAscending());
        }
    }
}

void TorrentListView::ShowHeaderContextMenu(wxCommandEvent& evt)
{
    wxMenu menu;

    for (size_t i = 0; i < GetColumnCount(); i++)
    {
        auto col = GetColumnAt(i);
        auto item = menu.Append(wxID_HIGHEST + i, col->GetTitle());
        item->SetCheckable(true);
        item->Check(!col->IsHidden());
    }

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& evt)
        {
            auto col = GetColumnAt(evt.GetId() - wxID_HIGHEST);
            col->SetHidden(!col->IsHidden());
        });

    PopupMenu(&menu);
}
