#include "textoutputdialog.hpp"

#include "../translator.hpp"

using pt::UI::Dialogs::TextOutputDialog;

TextOutputDialog::TextOutputDialog(wxWindow* parent, wxWindowID id, std::wstring const& title, std::wstring const& desc)
    : wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    m_outputText(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxTE_MULTILINE))
{
    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok = new wxButton(this, wxID_OK);
    ok->SetDefault();

    buttonsSizer->Add(ok);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->AddSpacer(FromDIP(11));
    mainSizer->Add(new wxStaticText(this, wxID_ANY, desc), 0, wxLEFT | wxRIGHT, FromDIP(11));
    mainSizer->AddSpacer(FromDIP(5));
    mainSizer->Add(m_outputText, 1, wxLEFT | wxRIGHT | wxEXPAND, FromDIP(11));
    mainSizer->AddSpacer(FromDIP(7));
    mainSizer->Add(buttonsSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, FromDIP(11));

    this->SetSizerAndFit(mainSizer);
    this->SetSize(FromDIP(wxSize(400, 250)));

    m_outputText->SetFocus();
    m_outputText->SetFont(
        wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));
}

TextOutputDialog::~TextOutputDialog()
{
}

void TextOutputDialog::SetOutputText(std::string const& text)
{
    m_outputText->SetValue(text);
    m_outputText->SetInsertionPointEnd();
}
