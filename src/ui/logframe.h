#ifndef _PT_UI_LOGFRAME_H
#define _PT_UI_LOGFRAME_H

#include <wx/frame.h>

class wxTextCtrl;

class LogFrame : public wxFrame
{
public:
    LogFrame(wxWindow* parent);

    void AppendLog(const wxString& message);

protected:
    void OnClose(wxCloseEvent& event);

private:
    wxTextCtrl* text_;

    wxDECLARE_EVENT_TABLE();
};

#endif
