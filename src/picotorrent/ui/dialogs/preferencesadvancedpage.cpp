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

#define MAKE_PROP(t1, t2, t3, key, label, description) \
    { \
        label, \
        { \
            []() { return new wx ##t1 ##Property(label); },\
            description, \
            [](Configuration* cfg, wxPGProperty* prop) { prop->SetValue(cfg->Get<t3>(key).value()); }, \
            [](Configuration* cfg, wxPGProperty* prop) { cfg->Set(key, prop->GetValue().Get ##t2()); } \
        } \
    }

static std::map<std::string, std::map<std::string, Property>> properties =
{
    {
        "libtorrent",
        {
            MAKE_PROP(Int,  Integer, int,  "libtorrent.active_checking", "active_checking", "The limit of number of simultaneous checking torrents."),
            MAKE_PROP(Int,  Integer, int,  "libtorrent.active_dht_limit", "active_dht_limit", "The max number of torrents to announce to the DHT."),
            MAKE_PROP(Int,  Integer, int,  "libtorrent.active_lsd_limit", "active_lsd_limit", "The max number of torrents to announce to the local network over the local service discovery protocol."),
            MAKE_PROP(Int,  Integer, int,  "libtorrent.active_tracker_limit", "active_tracker_limit", "The max number of torrents to announce to their trackers."),
            MAKE_PROP(Bool, Bool,    bool, "libtorrent.announce_to_all_tiers", "announce_to_all_tiers", "Controls how multi tracker torrents are treated. When this is set to true, one tracker from each tier is announced to. This is the uTorrent behavior. To be compliant with the Multi-tracker specification, set it to false."),
            MAKE_PROP(Bool, Bool,    bool, "libtorrent.announce_to_all_trackers", "announce_to_all_trackers", "Controls how multi tracker torrents are treated. If this is set to true, all trackers in the same tier are announced to in parallel. If all trackers in tier 0 fails, all trackers in tier 1 are announced as well. If it's set to false, the behavior is as defined by the multi tracker specification."),
            MAKE_PROP(Bool, Bool,    bool, "libtorrent.anonymous_mode", "anonymous_mode", "When set to true, the client tries to hide its identity to a certain degree. The user-agent will be reset to an empty string (except for private torrents). Trackers will only be used if they are using a proxy server. The listen sockets are closed, and incoming connections will only be accepted through a SOCKS5 or I2P proxy (if a peer proxy is set up and is run on the same machine as the tracker proxy). Since no incoming connections are accepted, NAT-PMP, UPnP, DHT and local peer discovery are all turned off when this setting is enabled. If you're using I2P, it might make sense to enable anonymous mode as well."),
            MAKE_PROP(Int,  Integer, int,  "libtorrent.stop_tracker_timeout", "stop_tracker_timeout", "The number of seconds to wait when sending a stopped message before considering a tracker to have timed out. This is usually shorter, to make the client quit faster. If the value is set to 0, the connections to trackers with the stopped event are suppressed."),
        }
    },
    {
        "PicoTorrent",
        {
            MAKE_PROP(Int,  Integer, int,  "save_resume_data_interval",   "save_resume_data_interval", "The interval (in seconds) between checks to save resume data for torrents. Saving resume data will help keep a current state if (for example) the application exits unexpectedly."),
            MAKE_PROP(Int,  Integer, int,  "ui.torrent_overview.columns", "torrent_overview_columns",  "The number of columns to show in the torrent overview panel."),
            MAKE_PROP(Bool, Bool,    bool, "ui.torrent_overview.show_piece_progress", "torrent_overview_show_piece_progress",  "When set to true, show the piece progress bar in the torrent overview panel.")
        }
    }
};

PreferencesAdvancedPage::PreferencesAdvancedPage(wxWindow* parent, std::shared_ptr<pt::Core::Configuration> cfg)
    : wxPanel(parent),
    m_cfg(cfg)
{
    m_grid = new wxPropertyGridManager(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);

    m_page = m_grid->AddPage();

    for (auto& [category, props] : properties)
    {
        m_page->Append(new wxPropertyCategory(category));

        for (auto& [key, prop] : props)
        {
            auto p = prop.ctor();

            // Set prop value
            prop.get(
                m_cfg.get(),
                p);

            p->SetHelpString(prop.help);
            m_page->Append(p);
        }
    }

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_grid, 1, wxEXPAND | wxALL);

    this->SetSizerAndFit(sizer);
}

void PreferencesAdvancedPage::Save()
{
    for (auto& [cat, props] : properties)
    {
        for (auto& [key, prop] : props)
        {
            prop.set(
                m_cfg.get(),
                m_page->GetProperty(key));
        }
    }
}
