#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxAppProgressIndicator;
class wxDataViewEvent;
class wxSplitterWindow;

namespace pt
{
    struct ApplicationOptions;
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

        void HandleOptions(std::shared_ptr<ApplicationOptions> options);

    private:
        enum
        {
            ptID_TORRENT_LIST_VIEW = wxID_HIGHEST + 1,
            ptID_MAIN_TIMER
        };

        void OnClose(wxCloseEvent&);
        void OnIconize(wxIconizeEvent&);
        void OnSessionAlert();
        void OnTimer(wxTimerEvent&);
        void OnTorrentContextMenu(wxDataViewEvent&);
        void OnTorrentSelectionChanged(wxDataViewEvent&);

        wxDECLARE_EVENT_TABLE();

        wxSplitterWindow* m_splitter;
        wxTimer* m_timer;
        StatusBar* m_status;
        TorrentListView* m_torrentListView;
        TorrentListViewModel* m_torrentListViewModel;
        TorrentDetailsView* m_torrentDetailsView;

        std::shared_ptr<Configuration> m_config;
        std::shared_ptr<Environment> m_env;
        std::shared_ptr<SessionState> m_state;
        std::shared_ptr<TaskBarIcon> m_taskBar;
        std::shared_ptr<Translator> m_trans;
    };
}
