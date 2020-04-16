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
    class TorrentListView : public wxDataViewCtrl
    {
    public:
        TorrentListView(wxWindow* parent, wxWindowID id);
        virtual ~TorrentListView();

        virtual wxSize GetMinSize() const wxOVERRIDE;

        void Sort();

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
    };
}
}
