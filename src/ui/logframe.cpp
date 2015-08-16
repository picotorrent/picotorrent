#include "logframe.h"

#include <wx/textctrl.h>

wxBEGIN_EVENT_TABLE(LogFrame, wxFrame)
    EVT_CLOSE(LogFrame::OnClose)
wxEND_EVENT_TABLE()

LogFrame::LogFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, wxT("Log"))
{
    text_ = new wxTextCtrl(this,
        wxID_ANY,
        wxEmptyString,
        wxDefaultPosition,
        wxDefaultSize,
        wxTE_MULTILINE);
}

void LogFrame::AppendLog(const wxString& message)
{
    text_->AppendText(message);
    text_->AppendText("\n");
}

void LogFrame::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto())
    {
        Hide();
        event.Veto();
    }
}
