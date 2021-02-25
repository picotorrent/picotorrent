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

    private:
        enum
        {
            ptID_KEY_SELECT_ALL = wxID_HIGHEST,
            ptID_KEY_DELETE,
            ptID_KEY_DELETE_FILES,
        };

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
