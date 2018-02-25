#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    struct SessionState;
    class Translator;

    class TorrentContextMenu : public wxMenu
    {
    public:
        TorrentContextMenu(wxWindow* parent,
            std::shared_ptr<Translator> translator,
            std::shared_ptr<SessionState> state);

    private:
        enum
        {
            ptID_RESUME = wxID_HIGHEST + 1,
            ptID_PAUSE,
            ptID_MOVE,
            ptID_REMOVE,
            ptID_QUEUE_UP,
            ptID_QUEUE_DOWN,
            ptID_QUEUE_TOP,
            ptID_QUEUE_BOTTOM,
            ptID_COPY_INFO_HASH,
            ptID_OPEN_IN_EXPLORER,
            ptID_FORCE_RECHECK
        };

        wxDECLARE_EVENT_TABLE();

        void CopyInfoHash(wxCommandEvent&);
        void ForceRecheck(wxCommandEvent&);
        void Move(wxCommandEvent&);
        void OpenInExplorer(wxCommandEvent&);
        void Pause(wxCommandEvent&);
        void Remove(wxCommandEvent&);
        void Resume(wxCommandEvent&);
        void QueueUp(wxCommandEvent&);
        void QueueDown(wxCommandEvent&);
        void QueueTop(wxCommandEvent&);
        void QueueBottom(wxCommandEvent&);

        wxWindow* m_parent;
        std::shared_ptr<SessionState> m_state;
        std::shared_ptr<Translator> m_trans;
    };
}
