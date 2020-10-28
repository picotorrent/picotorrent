#include "addtrackerdialog.hpp"

#include <wx/clipbrd.h>

#include "../translator.hpp"

using pt::UI::Dialogs::AddTrackerDialog;

AddTrackerDialog::AddTrackerDialog(wxWindow* parent, wxWindowID id)
    : wxDialog(parent, id, i18n("add_tracker"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    m_url(new wxTextCtrl(this, wxID_ANY)),
    m_tier(new wxTextCtrl(this, wxID_ANY, "1", wxDefaultPosition, wxSize(parent->FromDIP(30), -1), wxTE_RIGHT))
{
    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok = new wxButton(this, wxID_OK, i18n("ok"));
    ok->SetDefault();

    buttonsSizer->Add(ok);
    buttonsSizer->Add(new wxButton(this, wxID_CANCEL, i18n("cancel")), 0, wxLEFT, FromDIP(7));

    auto formSizer = new wxFlexGridSizer(2, FromDIP(7), FromDIP(10));
    formSizer->AddGrowableCol(1, 1);
    formSizer->Add(new wxStaticText(this, wxID_ANY, i18n("url")), 0);
    formSizer->Add(m_url, 1, wxEXPAND);
    formSizer->Add(new wxStaticText(this, wxID_ANY, i18n("tier")), 0);
    formSizer->Add(m_tier, 0);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(formSizer, 1, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, FromDIP(11));
    sizer->AddSpacer(FromDIP(7));
    sizer->Add(buttonsSizer, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(11));

    this->SetSizerAndFit(sizer);
    this->SetSize(FromDIP(wxSize(350, 80)));

    m_url->SetFocus();
    m_url->SetFont(
        wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));

    if (auto clipboard = wxClipboard::Get())
    {
        if (clipboard->Open())
        {
            wxTextDataObject data;

            if (clipboard->GetData(data))
            {
                std::string d = data.GetText().ToStdString();

                auto colIdx = d.find_first_of(':');
                bool maybeUrl = colIdx != std::string::npos
                    && (d.substr(0, colIdx) == "http"
                        || d.substr(0, colIdx) == "https"
                        || d.substr(0, colIdx) == "udp");


                if (maybeUrl)
                {
                    m_url->SetValue(d);
                    m_url->SetInsertionPointEnd();
                }
            }

            clipboard->Close();
        }
    }
}

AddTrackerDialog::~AddTrackerDialog()
{
}

int AddTrackerDialog::GetTier()
{
    return std::stoi(m_tier->GetValue().ToStdString());
}

std::string AddTrackerDialog::GetUrl()
{
    return m_url->GetValue().ToStdString();
}
