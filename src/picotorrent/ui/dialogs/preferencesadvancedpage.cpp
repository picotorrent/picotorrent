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
        label, \
        { \
            []() { return new wx ##type ##Property(label); },\
            description, \
            [](Configuration* cfg, wxPGProperty* prop) { prop->SetValue(cfg->Get##type(prop->GetLabel().ToStdString())); }, \
            [](Configuration* cfg, wxPGProperty* prop) { cfg->Set##type(prop->GetLabel().ToStdString(), prop->GetValue()); } \
        } \
    }

static std::map<std::string, Property> properties =
{
    MAKE_PROP(Bool, "anonymous_mode", "When set to true, the client tries to hide its identity to a certain degree. The user-agent will be reset to an empty string (except for private torrents). Trackers will only be used if they are using a proxy server. The listen sockets are closed, and incoming connections will only be accepted through a SOCKS5 or I2P proxy (if a peer proxy is set up and is run on the same machine as the tracker proxy). Since no incoming connections are accepted, NAT-PMP, UPnP, DHT and local peer discovery are all turned off when this setting is enabled. If you're using I2P, it might make sense to enable anonymous mode as well."),
};

PreferencesAdvancedPage::PreferencesAdvancedPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg)
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

    for (auto& [key,prop] : properties)
    {
        auto p = prop.ctor();

        // Set prop value
        prop.get(
            m_cfg.get(),
            p);

        p->SetHelpString(prop.help);
        m_page->Append(p);
    }

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_grid, 1, wxEXPAND | wxALL);

    this->SetSizerAndFit(sizer);
}

void PreferencesAdvancedPage::Save()
{
    for (auto& [key, prop] : properties)
    {
        prop.set(
            m_cfg.get(),
            m_page->GetProperty(key));
    }
}
