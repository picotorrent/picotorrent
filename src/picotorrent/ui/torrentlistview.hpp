#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>

#include <wx/dataview.h>

namespace pt
{
namespace UI
{
namespace Models
{
    class TorrentListModel;
}
    class TorrentListView : public wxDataViewCtrl
    {
    public:
        TorrentListView(wxWindow* parent, wxWindowID id, Models::TorrentListModel* model);
        virtual ~TorrentListView();

        virtual wxSize GetMinSize() const wxOVERRIDE;

    private:
        void ShowHeaderContextMenu(wxCommandEvent&);

        struct ColumnMetadata
        {
            ColumnMetadata(wxDataViewColumn* c, bool hide = false)
                : hidden(hide), column(c)
            {
            }

            bool hidden;
            wxDataViewColumn* column;
        };

        std::vector<ColumnMetadata> m_columns;
        Models::TorrentListModel* m_model;
    };
}
}
