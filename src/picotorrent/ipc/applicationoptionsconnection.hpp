#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/ipc.h>

namespace pt
{
    class MainFrame;

namespace ipc
{
    class ApplicationOptionsConnection : public wxConnection
    {
    public:
        ApplicationOptionsConnection(MainFrame* frame);
        virtual bool OnExecute(const wxString& topic, const void *data, size_t size, wxIPCFormat format) wxOVERRIDE;

    private:
        MainFrame* m_frame;
    };
}
}
