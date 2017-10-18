#include "connectionpage.hpp"

#include "translator.hpp"

using pt::ConnectionPage;

ConnectionPage::ConnectionPage(wxWindow* parent, std::shared_ptr<pt::Translator> translator)
    : wxPanel(parent, wxID_ANY)
{
	wxStaticBoxSizer* listenSizer = new wxStaticBoxSizer(wxVERTICAL, this, translator->Translate("listen_interface"));
	wxFlexGridSizer* listenGrid = new wxFlexGridSizer(1, 10, 10);

	m_listenInterfaces = new wxTextCtrl(listenSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, 60), wxTE_MULTILINE);

	listenGrid->AddGrowableCol(0, 1);
	listenGrid->Add(m_listenInterfaces, 1, wxEXPAND);
	listenSizer->Add(listenGrid, 1, wxEXPAND | wxALL, 5);

	wxStaticBoxSizer* privacySizer = new wxStaticBoxSizer(wxVERTICAL, this, translator->Translate("privacy"));
	wxFlexGridSizer* privacyGrid = new wxFlexGridSizer(1, 10, 10);
	privacyGrid->AddGrowableCol(0, 1);
	privacyGrid->Add(new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, translator->Translate("require_encryption_incoming")), 1, wxEXPAND);
	privacyGrid->Add(new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, translator->Translate("require_encryption_outgoing")), 1, wxEXPAND);
	privacySizer->Add(privacyGrid, 1, wxEXPAND | wxALL, 5);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(listenSizer, 0, wxEXPAND);
	sizer->AddSpacer(10);
	sizer->Add(privacySizer, 0, wxEXPAND);
	sizer->AddStretchSpacer();

	this->SetSizerAndFit(sizer);
}
