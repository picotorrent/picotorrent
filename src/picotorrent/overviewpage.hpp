#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace libtorrent
{
    struct torrent_status;
}

namespace pt
{
    class Translator;

    class OverviewPage : public wxPanel
    {
    public:
        OverviewPage(wxWindow* parent, wxWindowID id, std::shared_ptr<Translator> translator);
        void Update(libtorrent::torrent_status const& ts);

    private:
        wxStaticText* GetBoldStatic(wxString const& label);

        wxStaticText* m_name;
        wxStaticText* m_infoHash;
        wxStaticText* m_savePath;
        wxStaticText* m_pieces;
    };
}
