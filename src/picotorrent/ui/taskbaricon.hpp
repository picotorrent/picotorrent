#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/taskbar.h>

namespace pt
{
namespace UI
{
    class TaskBarIcon : public wxTaskBarIcon
    {
    public:
        TaskBarIcon(wxWindow* parent);

        void Hide();
        void Show();

    private:
        wxMenu* CreatePopupMenu() wxOVERRIDE;

        wxWindow* m_parent;
        wxIcon m_icon;
    };
}
}
