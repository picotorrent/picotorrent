#include "preferencesadvancedpage.hpp"

#include <map>

#include <wx/propgrid/manager.h>
#include <wx/propgrid/propgrid.h>

#include "../../core/configuration.hpp"

using pt::Core::Configuration;
using pt::UI::Dialogs::PreferencesAdvancedPage;

struct Property
{
    std::function<wxPGProperty*()> ctor;
    std::string help;
    std::function<void(Configuration*, wxPGProperty*)> get;
    std::function<void(Configuration*, wxPGProperty*)> set;
};

#define MAKE_PROP(type, label, description) \
    { \
        #label, \
        { \
            []() { return new wx ##type ##Property(label); },\
            description, \
            [](Configuration* cfg, wxPGProperty* prop) { prop->SetValue(cfg->Get##type(prop->GetLabel().ToStdString())); }, \
            [](Configuration* cfg, wxPGProperty* prop) { cfg->Set##type(prop->GetLabel().ToStdString(), prop->GetValue()); } \
        } \
    }

static std::map<std::string, Property> properties =
{
    MAKE_PROP(String, "user_agent", "The client identification to the tracker. The recommended format of this string is: \"client-name/client-version libtorrent/libtorrent-version\". This name will not only be used when making HTTP requests, but also when sending extended headers to peers that support that extension. It may not contain \\r or \\n."),
    MAKE_PROP(String, "announce_ip", "The IP address passed along to trackers as the '&ip=' parameter. If left as the default, that parameter is omitted.\n\nThis setting is only meant for very special cases where a seed is running on the same host as the tracker, and the tracker accepts the IP parameter (which normal trackers don't). Do not set this option unless you also control the tracker."),
    MAKE_PROP(Bool,   "allow_multiple_connections_per_ip", "Determines if connections from the same IP address as existing connections should be rejected or not. Rejecting multiple connections from the same IP address will prevent abusive behavior by peers. The logic for determining whether connections are to the same peer is more complicated with this enabled, and more likely to fail in some edge cases. It is not recommended to enable this feature."),
    MAKE_PROP(Bool,   "send_redundant_have", "Controls if have messages will be sent to peers that already have the piece. This is typically not necessary, but it might be necessary for collecting statistics in some cases."),
    MAKE_PROP(Bool,   "use_dht_as_fallback", "Determines how the DHT is used. If this is true, the DHT will only be used for torrents where all trackers in its tracker list has failed. Either by an explicit error message or a time out. If this is false, the DHT is used regardless of if the trackers fail or not."),
    MAKE_PROP(Bool,   "upnp_ignore_nonrouters", "Indicates whether or not the UPnP implementation should ignore any broadcast response from a device whose address is not on our subnet. i.e. it's a way to not talk to other people's routers by mistake."),
    MAKE_PROP(Bool,   "use_parole_mode", "Specifies if parole mode should be used. Parole mode means that peers that participate in pieces that fail the hash check are put in a mode where they are only allowed to download whole pieces. If the whole piece a peer in parole mode fails the hash check, it is banned. If a peer participates in a piece that passes the hash check, it is taken out of parole mode."),
    MAKE_PROP(Bool,   "auto_manage_prefer_seeds", "If true, prefer seeding torrents when determining which torrents to give active slots to. If false, give preference to downloading torrents."),
    MAKE_PROP(Bool,   "dont_count_slow_torrents", "If true, torrents without any payload transfers are not subject to the 'active_seeds' and 'active_downloads' limits. This is intended to make it more likely to utilize all available bandwidth, and avoid having torrents that don't transfer anything block the active slots."),
    MAKE_PROP(Bool,   "close_redundant_connections", "Specifies whether libtorrent should close connections where both ends have no utility in keeping the connection open. For instance if both ends have completed their downloads, there's no point in keeping it open."),
};

PreferencesAdvancedPage::PreferencesAdvancedPage(wxWindow* parent)
    : wxPanel(parent)
{
    m_grid = new wxPropertyGridManager(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);

    wxPropertyGridPage* page = m_grid->AddPage();

    for (auto [key,prop] : properties)
    {
        auto p = prop.ctor();
        p->SetHelpString(prop.help);
        page->Append(p);
    }

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_grid, 1, wxEXPAND | wxALL);

    this->SetSizerAndFit(sizer);
}
