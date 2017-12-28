#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <wx/propdlg.h>

namespace pt
{
    class Configuration;
    class DownloadsPage;
    class GeneralPage;
    class Translator;

    class PreferencesDialog : public wxPropertySheetDialog
    {
    public:
        PreferencesDialog(wxWindow* parent, std::shared_ptr<Configuration> cfg, std::shared_ptr<Translator> translator);

    private:
        wxDECLARE_EVENT_TABLE();

        void OnOk(wxCommandEvent&);

        DownloadsPage* m_downloads;
        GeneralPage* m_general;
    };
}
