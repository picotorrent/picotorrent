#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>

#include <libtorrent/fwd.hpp>

namespace pt
{
namespace UI
{
namespace Dialogs
{
    class AddMagnetLinkDialog : public wxDialog
    {
    public:
        AddMagnetLinkDialog(wxWindow* parent, wxWindowID id);
        virtual ~AddMagnetLinkDialog();

        std::vector<libtorrent::add_torrent_params> GetParams();

    private:
        bool IsMagnetLinkOrInfoHash(wxString const&);

        wxTextCtrl* m_links;
    };
}
}
}
