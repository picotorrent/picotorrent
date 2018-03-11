#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxDirPickerCtrl;
class wxFileDirPickerEvent;

namespace pt
{
    class Configuration;
    struct Preset;
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
            ptID_EDIT_PRESETS,
            ptID_SAVE_PATH,
            ptID_MOVE_COMPLETED,
            ptID_MOVE_COMPLETED_PATH
        };

        void LoadPreset(size_t index);
        void OnEditPresets(wxCommandEvent&);
        void OnMoveCompletedChanged(wxCommandEvent&);
        void OnMoveCompletedPathChanged(wxFileDirPickerEvent&);
        void OnPresetSelectionChanged(wxCommandEvent&);
        void OnSavePathChanged(wxFileDirPickerEvent&);
        void ReloadPresets();

        wxDECLARE_EVENT_TABLE();

        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Translator> m_translator;
        std::vector<Preset> m_presets;

        wxStaticBoxSizer* m_settingsSizer;
        wxChoice* m_preset;
        wxDirPickerCtrl* m_savePath;
        wxCheckBox* m_moveCompleted;
        wxDirPickerCtrl* m_moveCompletedPath;
    };
}
