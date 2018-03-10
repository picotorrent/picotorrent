#include "presetspage.hpp"

#include <wx/dataview.h>
#include <wx/filepicker.h>

#include "clientdata.hpp"
#include "config.hpp"
#include "editpresetsdlg.hpp"
#include "preset.hpp"
#include "translator.hpp"

using pt::PresetsPage;

wxBEGIN_EVENT_TABLE(PresetsPage, wxPanel)
    EVT_BUTTON(ptID_EDIT_PRESETS, PresetsPage::OnEditPresets)
    EVT_CHOICE(ptID_PRESETS, PresetsPage::OnPresetSelectionChanged)
wxEND_EVENT_TABLE()

PresetsPage::PresetsPage(wxWindow* parent, std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, wxID_ANY),
    m_cfg(cfg),
    m_translator(tr)
{
    wxStaticBoxSizer* presetsSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, i18n(tr, "presets"));
    m_preset = new wxChoice(presetsSizer->GetStaticBox(), ptID_PRESETS);
    presetsSizer->Add(m_preset, 1, wxEXPAND | wxALL, 5);
    presetsSizer->Add(new wxButton(presetsSizer->GetStaticBox(), ptID_EDIT_PRESETS, "...", wxDefaultPosition, wxSize(25, 25)), 0, wxALL, 5);

    // Preset settings
    m_settingsSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "transfers"));
    wxFlexGridSizer* settingsGrid = new wxFlexGridSizer(2, 10, 10);

    m_savePath = new wxDirPickerCtrl(m_settingsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);
    m_moveCompleted = new wxCheckBox(m_settingsSizer->GetStaticBox(), wxID_ANY, i18n(tr, "move_completed_downloads"));
    m_moveCompletedPath = new wxDirPickerCtrl(m_settingsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);
    m_moveCompletedPath->Enable(false);

    m_moveCompleted->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&)
    {
        m_moveCompletedPath->Enable(m_moveCompleted->IsChecked());
    });

    settingsGrid->AddGrowableCol(1, 1);
    settingsGrid->Add(new wxStaticText(m_settingsSizer->GetStaticBox(), wxID_ANY, i18n(tr, "save_path")), 0, wxALIGN_CENTER_VERTICAL);
    settingsGrid->Add(m_savePath, 1, wxEXPAND);
    m_settingsSizer->Add(settingsGrid, 1, wxEXPAND | wxALL, 5);
    m_settingsSizer->Add(m_moveCompleted, 0, wxALL, 5);
    m_settingsSizer->Add(m_moveCompletedPath, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(presetsSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(m_settingsSizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);

    ReloadPresets();
    
    if (m_preset->GetCount() > 0)
    {
        m_preset->SetSelection(0);
        LoadPreset(0);
    }
}

void PresetsPage::ApplyConfiguration()
{
    SaveCurrentPreset();
}

bool PresetsPage::ValidateConfiguration(wxString& error)
{
    return true;
}

void PresetsPage::LoadPreset(size_t index)
{
    bool found;
    Preset& p = m_cfg->Presets()->GetByIndex(index, &found);

    if (found)
    {
        m_currentPresetIndex = index;

        m_settingsSizer->GetStaticBox()->Enable(true);

        m_savePath->SetPath(p.save_path.string());
        m_moveCompleted->SetValue(p.move_completed_downloads);
        m_moveCompletedPath->Enable(m_moveCompleted->GetValue());
        m_moveCompletedPath->SetPath(p.move_completed_path.string());
    }
}

void PresetsPage::OnEditPresets(wxCommandEvent&)
{
    SaveCurrentPreset();

    EditPresetsDialog dlg(this, m_cfg, m_translator);
    dlg.ShowModal();

    ReloadPresets();

    if (m_preset->GetCount() > 0)
    {
        m_preset->SetSelection(0);
        LoadPreset(0);
    }
}

void PresetsPage::OnPresetSelectionChanged(wxCommandEvent& e)
{
    SaveCurrentPreset();

    m_currentPresetIndex = m_preset->GetSelection();
    LoadPreset(m_currentPresetIndex);
}

void PresetsPage::ReloadPresets()
{
    m_preset->Clear();
    m_settingsSizer->GetStaticBox()->Enable(false);

    m_savePath->SetPath("");
    m_moveCompleted->SetValue(false);
    m_moveCompletedPath->SetPath("");

    std::vector<Preset> presets = m_cfg->Presets()->GetAll();

    for (size_t i = 0; i < presets.size(); i++)
    {
        m_preset->Append(presets.at(i).name, new ClientData<size_t>(i));
    }
}

void PresetsPage::SaveCurrentPreset()
{
    bool found;
    Preset& p = m_cfg->Presets()->GetByIndex(m_currentPresetIndex, &found);

    if (found)
    {
        p.move_completed_downloads = m_moveCompleted->GetValue();
        p.move_completed_path = m_moveCompletedPath->GetPath().ToStdString();
        p.save_path = m_savePath->GetPath().ToStdString();

        m_cfg->Presets()->Update(m_currentPresetIndex, p);
    }
}
