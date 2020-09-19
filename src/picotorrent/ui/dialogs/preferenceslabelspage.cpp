#include "preferenceslabelspage.hpp"

#include "../clientdata.hpp"
#include "../../core/configuration.hpp"
#include "../translator.hpp"

using pt::Core::Configuration;
using pt::UI::Dialogs::PreferencesLabelsPage;

PreferencesLabelsPage::PreferencesLabelsPage(wxWindow* parent, std::shared_ptr<Configuration> cfg)
    : wxPanel(parent, wxID_ANY),
    m_cfg(cfg)
{
}

void PreferencesLabelsPage::Save()
{
}

bool PreferencesLabelsPage::IsValid()
{
    return true;
}
