#include "addmagnetlinkdialog.hpp"

#include <regex>

#include <libtorrent/magnet_uri.hpp>
#include <loguru.hpp>
#include <wx/clipbrd.h>
#include <wx/tokenzr.h>

#include "../translator.hpp"

namespace lt = libtorrent;
using pt::UI::Dialogs::AddMagnetLinkDialog;

AddMagnetLinkDialog::AddMagnetLinkDialog(wxWindow* parent, wxWindowID id)
    : wxDialog(parent, id, i18n("add_magnet_link_s")),
    m_links(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxTE_MULTILINE))
{
    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok = new wxButton(this, wxID_OK);
    ok->SetDefault();

    buttonsSizer->Add(ok);
    buttonsSizer->Add(new wxButton(this, wxID_CANCEL), 0, wxLEFT, FromDIP(7));

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->AddSpacer(FromDIP(11));
    mainSizer->Add(new wxStaticText(this, wxID_ANY, i18n("add_magnet_link_s_description")), 0, wxLEFT | wxRIGHT, FromDIP(11));
    mainSizer->AddSpacer(FromDIP(5));
    mainSizer->Add(m_links, 1, wxLEFT | wxRIGHT | wxEXPAND, FromDIP(11));
    mainSizer->AddSpacer(FromDIP(7));
    mainSizer->Add(buttonsSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, FromDIP(11));

    this->SetSizerAndFit(mainSizer);
    this->SetSize(FromDIP(wxSize(400, 250)));

    m_links->SetFont(
        wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));

    if (auto clipboard = wxClipboard::Get())
    {
        if (clipboard->Open())
        {
            wxTextDataObject data;

            if (clipboard->GetData(data))
            {
                wxString d = data.GetText();

                if (IsMagnetLinkOrInfoHash(d))
                {
                    if (!d.EndsWith("\n"))
                    {
                        d = d + "\n";
                    }

                    m_links->SetValue(d);
                    m_links->SetInsertionPointEnd();
                }
            }

            clipboard->Close();
        }
    }
}

AddMagnetLinkDialog::~AddMagnetLinkDialog()
{
}

std::vector<libtorrent::add_torrent_params> AddMagnetLinkDialog::GetParams()
{
    std::vector<lt::add_torrent_params> result;

    wxStringTokenizer tokenizer(m_links->GetValue());

    while (tokenizer.HasMoreTokens())
    {
        std::string token = tokenizer.GetNextToken();
        if (!IsMagnetLinkOrInfoHash(token)) { continue; }

        if (token.substr(0, 20) != "magnet:?xt=urn:btih:")
        {
            token = "magnet:?xt=urn:btih:" + token;
        }

        lt::error_code ec;
        lt::add_torrent_params params = lt::parse_magnet_uri(token, ec);

        if (ec)
        {
            // TODO log warning
            continue;
        }

        result.push_back(params);
    }

    return result;
}

bool AddMagnetLinkDialog::IsMagnetLinkOrInfoHash(wxString const& str)
{
    std::regex infoHash("[a-fA-F\\d]{40}", std::regex_constants::icase);

    if (std::regex_match(str.ToStdString(), infoHash))
    {
        return true;
    }

    if (str.StartsWith("magnet:?xt=urn:btih:"))
    {
        return true;
    }

    return false;
}
