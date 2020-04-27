#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxDirPickerCtrl;

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
    class PreferencesDownloadsPage : public wxPanel
    {
    public:
        PreferencesDownloadsPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);

        bool IsValid();
        void Save();

    private:
        std::shared_ptr<Core::Configuration> m_cfg;

        wxDirPickerCtrl* m_savePathCtrl;
        wxDirPickerCtrl* m_moveCompletedPathCtrl;
        wxCheckBox* m_moveCompletedEnabled;
        wxCheckBox* m_moveCompletedOnlyFromDefault;
        wxCheckBox* m_enableDownloadLimit;
        wxCheckBox* m_pauseLowDiskSpace;
        wxTextCtrl* m_downloadLimit;
        wxCheckBox* m_enableUploadLimit;
        wxTextCtrl* m_uploadLimit;
        wxTextCtrl* m_activeLimit;
        wxTextCtrl* m_activeDownloadsLimit;
        wxTextCtrl* m_activeSeedsLimit;
    };
}
}
}
