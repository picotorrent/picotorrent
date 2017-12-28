#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxDataViewEvent;
class wxSplitterWindow;

namespace pt
{
    class Configuration;
    class Environment;
    struct SessionState;
    class StatusBar;
    class TaskBarIcon;
    class TorrentDetailsView;
    class TorrentListView;
    class TorrentListViewModel;
    class Translator;

    class MainFrame : public wxFrame
    {
    public:
        MainFrame(std::shared_ptr<Configuration> config,
            std::shared_ptr<Environment> env,
            std::shared_ptr<Translator> translator);
        virtual ~MainFrame();

    private:
        enum
        {
            ptID_TORRENT_LIST_VIEW = wxID_HIGHEST + 1,
            ptID_MAIN_TIMER
        };

        void OnSessionAlert();
        void OnTimer(wxTimerEvent&);
        void OnTorrentContextMenu(wxDataViewEvent&);
        void OnTorrentSelectionChanged(wxDataViewEvent&);

        wxDECLARE_EVENT_TABLE();

        wxSplitterWindow* m_splitter;
        wxTimer* m_timer;
        StatusBar* m_status;
        TaskBarIcon* m_taskBar;
        TorrentListView* m_torrentListView;
        TorrentListViewModel* m_torrentListViewModel;
        TorrentDetailsView* m_torrentDetailsView;

        std::shared_ptr<Configuration> m_config;
        std::shared_ptr<Environment> m_env;
        std::shared_ptr<SessionState> m_state;
        std::shared_ptr<Translator> m_trans;
    };
}
