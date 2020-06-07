#include "aboutdialog.hpp"

#include <sstream>

#include <boost/version.hpp>
#include <fmt/core.h>
#include <libtorrent/version.hpp>
#include <openssl/opensslv.h>
#include <sqlite3.h>
#include <wx/hyperlink.h>
#include <wx/listctrl.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/version.h>

#include "../../buildinfo.hpp"
#include "../translator.hpp"

using pt::UI::Dialogs::AboutDialog;

AboutDialog::AboutDialog(wxWindow* parent, wxWindowID id)
    : wxDialog(parent, id, i18n("about_picotorrent"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxIcon icon(L"AppIcon", wxBITMAP_TYPE_ICO_RESOURCE, 128, 128);
    auto bmp = new wxStaticBitmap(this, wxID_ANY, icon);

    auto lv = new wxListView(this);
    lv->AppendColumn(i18n("component"), wxLIST_FORMAT_LEFT, FromDIP(200));
    lv->AppendColumn(i18n("version"), wxLIST_FORMAT_LEFT, FromDIP(80));
    lv->AppendColumn(i18n("commitish"), wxLIST_FORMAT_LEFT, FromDIP(80));

    std::stringstream boostVersion;
    boostVersion << BOOST_VERSION / 100000 << "."
        << BOOST_VERSION / 100 % 1000 << "."
        << BOOST_VERSION % 100;

    std::stringstream fmtVersion;
    fmtVersion << FMT_VERSION / 10000 << "."
        << FMT_VERSION % 10000 / 100 << "."
        << FMT_VERSION % 100;

    lv->InsertItem(lv->GetItemCount(), "Boost");
    lv->SetItem(lv->GetItemCount() - 1, 1, boostVersion.str());
    lv->SetItem(lv->GetItemCount() - 1, 2, "-");

    lv->InsertItem(lv->GetItemCount(), "fmt");
    lv->SetItem(lv->GetItemCount() - 1, 1, fmtVersion.str());
    lv->SetItem(lv->GetItemCount() - 1, 2, BOOST_STRINGIZE(PICO_FMT_GIT_COMMITISH));

    lv->InsertItem(lv->GetItemCount(), OPENSSL_VERSION_TEXT);
    lv->SetItem(lv->GetItemCount() - 1, 1, "-");
    lv->SetItem(lv->GetItemCount() - 1, 2, "-");

    lv->InsertItem(lv->GetItemCount(), "PicoTorrent");
    lv->SetItem(lv->GetItemCount() - 1, 1, BuildInfo::version());
    lv->SetItem(lv->GetItemCount() - 1, 2, BuildInfo::commitish());

    lv->InsertItem(lv->GetItemCount(), "Rasterbar-libtorrent");
    lv->SetItem(lv->GetItemCount() - 1, 1, LIBTORRENT_VERSION);
    lv->SetItem(lv->GetItemCount() - 1, 2, BOOST_STRINGIZE(PICO_LIBTORRENT_GIT_COMMITISH));

    lv->InsertItem(lv->GetItemCount(), "SQLite");
    lv->SetItem(lv->GetItemCount() - 1, 1, SQLITE_VERSION);
    lv->SetItem(lv->GetItemCount() - 1, 2, "-");

    lv->InsertItem(lv->GetItemCount(), "wxWidgets");
    lv->SetItem(lv->GetItemCount() - 1, 1, wxVERSION_NUM_DOT_STRING_T);
    lv->SetItem(lv->GetItemCount() - 1, 2, BOOST_STRINGIZE(PICO_WX_GIT_COMMITISH));

    auto desc = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    desc->AppendText(i18n("picotorrent_description"));

    auto footerSizer = new wxBoxSizer(wxHORIZONTAL);
    footerSizer->Add(new wxStaticText(this, wxID_ANY, "© 2015-2020"));
    footerSizer->AddStretchSpacer();
    footerSizer->Add(new wxHyperlinkCtrl(this, wxID_ANY, "https://picotorrent.org", "https://picotorrent.org?app"));

    auto border = FromDIP(11);
    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(bmp, 0, wxLEFT | wxRIGHT | wxTOP | wxCENTER, border);
    sizer->AddSpacer(FromDIP(11));
    sizer->Add(desc, 0, wxEXPAND | wxLEFT | wxRIGHT, border);
    sizer->AddSpacer(FromDIP(7));
    sizer->Add(lv, wxSizerFlags(1).Border(wxLEFT | wxRIGHT, border).Expand());
    sizer->AddSpacer(FromDIP(7));
    sizer->Add(footerSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, border);

    this->SetSizerAndFit(sizer);
}

AboutDialog::~AboutDialog()
{
}
