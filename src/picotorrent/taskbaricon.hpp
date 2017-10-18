#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/taskbar.h>

namespace pt
{
    class TaskBarIcon : public wxTaskBarIcon
    {
	public:
		TaskBarIcon(wxFrame* parent);

    private:
        wxMenu* CreatePopupMenu() wxOVERRIDE;

		wxFrame* m_parent;
    };
}
