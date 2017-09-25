#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace libtorrent
{
	class session;
}

class wxNotebook;
class wxSplitterWindow;

namespace pt
{
	class TorrentDetailsView;
	class TorrentListView;

    class MainFrame : public wxFrame
    {
    public:
		MainFrame();
		~MainFrame();

    private:
        wxDECLARE_EVENT_TABLE();

		wxSplitterWindow* m_splitter;
		TorrentListView* m_torrentListView;
		TorrentDetailsView* m_torrentDetailsView;

		std::shared_ptr<libtorrent::session> m_session;
    };
}
