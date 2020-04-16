#include "aboutdialog.hpp"

#include <wx/hyperlink.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>

using pt::UI::Dialogs::AboutDialog;

AboutDialog::AboutDialog(wxWindow* parent, wxWindowID id)
    : wxDialog(parent, id, wxEmptyString)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    // TODO icon

    auto picoTitle = new wxStaticText(this, wxID_ANY, "PicoTorrent");
    auto picoTitleFont = picoTitle->GetFont();
    picoTitleFont.SetPointSize(18);
    picoTitle->SetFont(picoTitleFont);

    auto border = FromDIP(5);

    sizer->Add(picoTitle, 0, wxALL | wxCENTER, border);
    sizer->Add(new wxStaticText(this, wxID_ANY, "© 2015-2020"), 0, wxALL | wxCENTER, border);
    sizer->AddStretchSpacer();
    sizer->Add(new wxStaticText(this, wxID_ANY, "Built with love, using Boost, Rasterbar-libtorrent, OpenSSL and wxWidgets."), 0, wxALL | wxCENTER, border);
    sizer->Add(new wxHyperlinkCtrl(this, wxID_ANY, "https://picotorrent.org", "https://picotorrent.org?app"), 0, wxALL | wxCENTER, border);

    this->SetSizerAndFit(sizer);
    // this->SetSize(FromDIP(wxSize(250, 300)));
    this->SetTitle("about picotorrent");
}

AboutDialog::~AboutDialog()
{
}
