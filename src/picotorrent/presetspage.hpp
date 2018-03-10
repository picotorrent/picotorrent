#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxDirPickerCtrl;

namespace pt
{
    class Configuration;
    class Translator;

    class PresetsPage : public wxPanel
    {
    public:
        PresetsPage(wxWindow* parent, std::shared_ptr<Configuration> config, std::shared_ptr<Translator> translator);

        void ApplyConfiguration();
        bool ValidateConfiguration(wxString& error);

    private:
        enum
        {
            ptID_PRESETS = wxID_HIGHEST + 3222,
            ptID_EDIT_PRESETS
        };

        void LoadPreset(size_t index);
        void OnEditPresets(wxCommandEvent&);
        void OnPresetSelectionChanged(wxCommandEvent&);
        void ReloadPresets();
        void SaveCurrentPreset();

        wxDECLARE_EVENT_TABLE();

        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Translator> m_translator;

        size_t m_currentPresetIndex;

        wxStaticBoxSizer* m_settingsSizer;
        wxChoice* m_preset;
        wxDirPickerCtrl* m_savePath;
        wxCheckBox* m_moveCompleted;
        wxDirPickerCtrl* m_moveCompletedPath;
    };
}
