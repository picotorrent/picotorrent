#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <vector>

namespace libtorrent
{
    struct add_torrent_params;
}

namespace pt
{
    struct SessionState;
    class Translator;

    class AddTorrentProcedure
    {
    public:
        AddTorrentProcedure(wxWindow* parent, std::shared_ptr<Translator> translator, std::shared_ptr<SessionState> state);
        void Execute();
        void ExecuteMagnet();

    private:
        void Execute(std::vector<libtorrent::add_torrent_params>& params);

        wxWindow* m_parent;
        std::shared_ptr<SessionState> m_state;
        std::shared_ptr<Translator> m_trans;
    };
}
