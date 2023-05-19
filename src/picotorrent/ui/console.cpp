#include "console.hpp"

#include "filters/pqltorrentfilter.hpp"
#include "ids.hpp"
#include "models/torrentlistmodel.hpp"
#include "torrentlistview.hpp"
#include "../core/configuration.hpp"

using pt::UI::Console;

wxDEFINE_EVENT(ptEVT_FILTER_CHANGED, wxCommandEvent);

Console::Console(wxWindow* parent, wxWindowID id, pt::UI::Models::TorrentListModel* model, bool isDarkMode)
    : wxPanel(parent, id),
    m_input(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT | wxTE_PROCESS_ENTER)),
    m_model(model)
{
    m_input->SetFont(
        wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));

    wxIcon funnel(isDarkMode ? L"ICO_TERMINAL_DARK_THEME" : L"ICO_TERMINAL", wxBITMAP_TYPE_ICO_RESOURCE, FromDIP(16), FromDIP(16));

    int i = FromDIP(16);
    printf("%d", i);

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(new wxStaticBitmap(this, wxID_ANY, funnel), 0, wxALIGN_CENTER | wxLEFT, FromDIP(4));
    sizer->Add(m_input, 1, wxEXPAND | wxALL, FromDIP(4));

    this->SetBackgroundColour(isDarkMode ? wxColour(32,32,32) : wxColour(255,255,255));
    this->SetSizerAndFit(sizer);

    this->Bind(
        wxEVT_TEXT_ENTER,
        [this](wxCommandEvent&)
        {
            CreateFilter(m_input->GetValue().ToStdString());
        });
}

void Console::SetText(std::string const& text)
{
    m_input->SetValue(text);
    CreateFilter(text);
}

void Console::CreateFilter(std::string const& input)
{
    if (input.empty())
    {
        m_model->ClearFilter();
    }
    else
    {
        std::string err;
        auto filter = Filters::PqlTorrentFilter::Create(input, &err);

        if (!filter && !err.empty())
        {
            wxMessageBox(err, "Filter error", wxICON_ERROR | wxOK, GetParent());
            return;
        }

        m_model->SetFilter(std::move(filter));
    }

    wxCommandEvent evt(ptEVT_FILTER_CHANGED);
    evt.SetString(input);

    wxPostEvent(GetParent(), evt);
}
