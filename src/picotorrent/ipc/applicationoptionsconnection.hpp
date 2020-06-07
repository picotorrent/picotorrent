#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/ipc.h>

namespace pt
{
namespace UI
{
    class MainFrame;
}
namespace IPC
{
    class ApplicationOptionsConnection : public wxConnection
    {
    public:
        ApplicationOptionsConnection(UI::MainFrame* frame);
        virtual bool OnExecute(const wxString& topic, const void *data, size_t size, wxIPCFormat format) wxOVERRIDE;

    private:
        UI::MainFrame* m_frame;
    };
}
}
