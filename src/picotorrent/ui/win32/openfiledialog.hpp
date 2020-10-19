#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include <tuple>
#include <vector>

struct IFileOpenDialog;

namespace pt::UI::Win32
{
    class OpenFileDialog
    {
    public:
        enum class Option
        {
            Multi
        };

        OpenFileDialog();
        ~OpenFileDialog();

        void GetFiles(std::vector<std::string>& files);

        void SetFileTypes(std::vector<std::tuple<std::wstring, std::wstring>> const& types);
        void SetOption(Option opt);
        void SetTitle(std::wstring const& title);
        void Show(wxWindow* parent);

    private:
        IFileOpenDialog* m_wrappedDialog;
    };
}
