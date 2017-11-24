#include "magnetlinkvalidator.hpp"

#include <libtorrent/magnet_uri.hpp>

const wxString MagnetPrefix = "magnet:?xt=urn:btih:";

namespace lt = libtorrent;
using pt::MagnetLinkValidator;

wxObject* MagnetLinkValidator::Clone() const
{
    return new MagnetLinkValidator();
}

bool MagnetLinkValidator::Validate(wxWindow* WXUNUSED(parent))
{
    wxTextCtrl* tc = wxDynamicCast(GetWindow(), wxTextCtrl);
    wxCHECK_MSG(tc, true, wxT("validator window must be wxTextCtrl"));

    wxString val = tc->GetValue();

    if (!val.StartsWith(MagnetPrefix))
    {
        val = MagnetPrefix + val;
    }

    lt::add_torrent_params params;
    lt::error_code ec;
    lt::parse_magnet_uri(val.ToStdString(), params, ec);

    if (ec)
    {
        wxMessageBox(ec.message(), "PicoTorrent");
        return false;
    }

    return true;
}
