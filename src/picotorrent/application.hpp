#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
    class MainFrame;

    class Application : public wxApp
    {
    public:
        Application();
        virtual bool OnInit();

    private:
        MainFrame* m_mainFrame;
    };
}
