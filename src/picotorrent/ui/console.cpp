#include "console.hpp"

#include "filters/pqltorrentfilter.hpp"
#include "ids.hpp"
#include "models/torrentlistmodel.hpp"
#include "torrentlistview.hpp"

using pt::UI::Console;

Console::Console(wxWindow* parent, wxWindowID id, Models::TorrentListModel* model)
    : wxPanel(parent, id),
    m_input(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT | wxTE_PROCESS_ENTER)),
    m_model(model)
{
    m_input->SetFont(
        wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));

    wxIcon funnel(L"ICO_FUNNEL", wxBITMAP_TYPE_ICO_RESOURCE, FromDIP(16), FromDIP(16));

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(new wxStaticBitmap(this, wxID_ANY, funnel), 0, wxALIGN_CENTER);
    sizer->Add(m_input, 1, wxEXPAND | wxALL, FromDIP(2));

    this->SetBackgroundColour(*wxWHITE);
    this->SetSizerAndFit(sizer);

    this->Bind(wxEVT_TEXT_ENTER, &Console::CreateFilter, this);
}

void Console::CreateFilter(wxCommandEvent&)
{
    std::string input = m_input->GetValue().ToStdString();

    if (input.empty())
    {
        m_model->ClearFilter();
        return;
    }

    auto filter = Filters::PqlTorrentFilter::Create(input);

    if (!filter)
    {
        // TODO: log
        return;
    }

    m_model->SetFilter(std::move(filter));
}
