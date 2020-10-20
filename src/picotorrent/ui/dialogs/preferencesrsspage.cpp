#include "preferencesrsspage.hpp"

using pt::UI::Dialogs::PreferencesRssPage;

PreferencesRssPage::PreferencesRssPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg)
    : m_cfg(cfg)
{
}

PreferencesRssPage::~PreferencesRssPage()
{
}

bool PreferencesRssPage::IsValid()
{
    return true;
}

void PreferencesRssPage::Save()
{
}
