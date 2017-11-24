#include "proxypage.hpp"
#include "translator.hpp"

using pt::ProxyPage;

ProxyPage::ProxyPage(wxWindow* parent, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, wxID_ANY)
{
    wxStaticBoxSizer* proxySizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "proxy"));
    wxFlexGridSizer* proxyGrid = new wxFlexGridSizer(2, 10, 10);
    proxyGrid->AddGrowableCol(1, 1);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "type")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(new wxChoice(proxySizer->GetStaticBox(), wxID_ANY), 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "host")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY), 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "port")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY), 1);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "username")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY), 1, wxEXPAND);
    proxyGrid->Add(new wxStaticText(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "password")), 0, wxALIGN_CENTER_VERTICAL);
    proxyGrid->Add(new wxTextCtrl(proxySizer->GetStaticBox(), wxID_ANY), 1, wxEXPAND);

    wxFlexGridSizer* proxySettingsGrid = new wxFlexGridSizer(2, 10, 10);
    proxyGrid->Add(new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "force_proxy")), 1, wxEXPAND);
    proxyGrid->Add(new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "proxy_hostnames")), 1, wxEXPAND);
    proxyGrid->Add(new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "proxy_peers")), 1, wxEXPAND);
    proxyGrid->Add(new wxCheckBox(proxySizer->GetStaticBox(), wxID_ANY, i18n(tr, "proxy_trackers")), 1, wxEXPAND);

    proxySizer->Add(proxyGrid, 1, wxEXPAND | wxALL, 5);
    proxySizer->Add(proxySettingsGrid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(proxySizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);
}
