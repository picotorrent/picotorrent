#include "peerslistview.hpp"

#include "peersviewmodel.hpp"
#include "translator.hpp"

#include "scaler.hpp"

using pt::PeersListView;

PeersListView::PeersListView(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
    : wxDataViewCtrl(parent, id)
{
    AppendTextColumn(i18n(tr, "ip"), PeersViewModel::Columns::IP, wxDATAVIEW_CELL_INERT, SX(110), wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE);
    AppendTextColumn(i18n(tr, "client"), PeersViewModel::Columns::Client, wxDATAVIEW_CELL_INERT, SX(140), wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE);
    AppendTextColumn(i18n(tr, "flags"), PeersViewModel::Columns::Flags, wxDATAVIEW_CELL_INERT, SX(80), wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
    AppendTextColumn(i18n(tr, "dl"), PeersViewModel::Columns::DownloadRate, wxDATAVIEW_CELL_INERT, SX(80), wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE);
    AppendTextColumn(i18n(tr, "ul"), PeersViewModel::Columns::UploadRate, wxDATAVIEW_CELL_INERT, SX(80), wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE);
    AppendProgressColumn(i18n(tr, "progress"), PeersViewModel::Columns::Progress, wxDATAVIEW_CELL_INERT, SX(100), wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE);

    // Ugly hack to prevent the last "real" column from stretching.
    AppendColumn(new wxDataViewColumn(wxEmptyString, new wxDataViewTextRenderer(), -1, 0));

}

void PeersListView::Sort()
{
    if(auto sortingColumn = GetSortingColumn())
    {
        auto model = reinterpret_cast<PeersViewModel*>(GetModel());
        
        if(HasSelection())
        {
            model->Sort(
                GetColumnIndex(sortingColumn),
                sortingColumn->IsSortOrderAscending());
        }
        else
        {
            model->Sort(
                GetColumnIndex(sortingColumn),
                sortingColumn->IsSortOrderAscending());
        }
    }
}
