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

    class DownloadsPage : public wxPanel
    {
    public:
        DownloadsPage(wxWindow* parent, std::shared_ptr<Configuration> cfg, std::shared_ptr<Translator> translator);

        void ApplyConfiguration();
        bool ValidateConfiguration(wxString& error);

    private:
        std::shared_ptr<Configuration> m_cfg;

        wxDirPickerCtrl* m_savePathCtrl;
        wxCheckBox* m_enableDownloadLimit;
        wxTextCtrl* m_downloadLimit;
        wxCheckBox* m_enableUploadLimit;
        wxTextCtrl* m_uploadLimit;
        wxTextCtrl* m_activeLimit;
        wxTextCtrl* m_activeDownloadsLimit;
        wxTextCtrl* m_activeSeedsLimit;
    };
}
