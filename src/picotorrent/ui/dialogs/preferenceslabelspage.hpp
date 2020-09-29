#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <memory>

class wxColourPickerCtrl;
class wxDirPickerCtrl;
class wxListView;

namespace pt
{
namespace Core
{
    class Configuration;
}
namespace UI
{
namespace Dialogs
{
    class PreferencesLabelsPage : public wxPanel
    {
    public:
        PreferencesLabelsPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);
        virtual ~PreferencesLabelsPage();

        bool IsValid();
        void Save();

    private:
        void EnableDisableAll(bool enabled);

        std::shared_ptr<Core::Configuration> m_cfg;
        std::vector<int32_t> m_removedLabels;

        wxListView* m_labelsList;
        wxTextCtrl* m_name;
        wxCheckBox* m_colorEnabled;
        wxColourPickerCtrl* m_colorPicker;
        wxDirPickerCtrl* m_savePath;
        wxCheckBox* m_savePathEnabled;
        wxTextCtrl* m_applyFilter;
        wxCheckBox* m_applyFilterEnabled;
    };
}
}
}
