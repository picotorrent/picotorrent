#include "torrentdetailsoverviewpanel.hpp"

#include <fmt/format.h>
#include <wx/clipbrd.h>
#include <wx/sizer.h>

#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "../core/utils.hpp"
#include "translator.hpp"

using pt::UI::TorrentDetailsOverviewPanel;

static wxStaticText* BoldLabel(wxWindow* parent, wxWindowID id, wxString const& text)
{
    auto s = new wxStaticText(parent, id, text);
    auto f = s->GetFont();
    f.SetWeight(wxFONTWEIGHT_BOLD);
    s->SetFont(f);
    return s;
}

std::wstring SecondsToFriendly(std::chrono::seconds secs)
{
    std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(secs);
    std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(secs - hours_left);
    std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(secs - hours_left - min_left);

    if (hours_left.count() <= 0)
    {
        if (min_left.count() <= 0)
        {
            return fmt::format(L"{0}s", sec_left.count());
        }

        return fmt::format(L"{0}m {1}s", min_left.count(), sec_left.count());
    }

    return fmt::format(
        L"{0}h {1}m {2}s",
        hours_left.count(),
        min_left.count(),
        sec_left.count());
}

class CopyableStaticText : public wxStaticText
{
public:
    CopyableStaticText(wxWindow* parent)
        : wxStaticText(parent, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END)
    {
        this->Bind(wxEVT_RIGHT_DOWN,
            [this](wxMouseEvent const&)
            {
                if (this->GetLabel() == "-")
                {
                    return;
                }

                wxMenu menu;
                menu.Append(9999, i18n("copy"));
                menu.Bind(wxEVT_MENU,
                    [this](wxCommandEvent const&)
                    {
                        if (wxTheClipboard->Open())
                        {
                            wxTheClipboard->SetData(new wxTextDataObject(this->GetLabel()));
                            wxTheClipboard->Close();
                        }
                    });

                PopupMenu(&menu);
            });
    }
};

TorrentDetailsOverviewPanel::TorrentDetailsOverviewPanel(wxWindow* parent, wxWindowID id)
    : wxScrolledWindow(parent, id),
    m_name(new CopyableStaticText(this)),
    m_infoHash(new CopyableStaticText(this)),
    m_savePath(new CopyableStaticText(this)),
    m_pieces(new CopyableStaticText(this)),
    m_comment(new CopyableStaticText(this)),
    m_size(new CopyableStaticText(this)),
    m_priv(new CopyableStaticText(this)),
    m_ratio(new CopyableStaticText(this)),
    m_lastDownload(new CopyableStaticText(this)),
    m_lastUpload(new CopyableStaticText(this)),
    m_totalDownload(new CopyableStaticText(this)),
    m_totalUpload(new CopyableStaticText(this))
{
    auto sizer = new wxFlexGridSizer(4, FromDIP(10), FromDIP(10));
    sizer->AddGrowableCol(1, 1);
    sizer->AddGrowableCol(3, 1);

    sizer->Add(BoldLabel(this, wxID_ANY, i18n("name")));
    sizer->Add(m_name, 1, wxEXPAND);
    sizer->Add(BoldLabel(this, wxID_ANY, i18n("info_hash")));
    sizer->Add(m_infoHash, 1, wxEXPAND);

    sizer->Add(BoldLabel(this, wxID_ANY, i18n("save_path")));
    sizer->Add(m_savePath, 1, wxEXPAND);
    sizer->Add(BoldLabel(this, wxID_ANY, i18n("pieces")));
    sizer->Add(m_pieces, 1, wxEXPAND);

    sizer->Add(BoldLabel(this, wxID_ANY, i18n("comment")));
    sizer->Add(m_comment, 1, wxEXPAND);
    sizer->Add(BoldLabel(this, wxID_ANY, i18n("size")));
    sizer->Add(m_size, 1, wxEXPAND);

    sizer->Add(BoldLabel(this, wxID_ANY, i18n("private")));
    sizer->Add(m_priv, 1, wxEXPAND);
    sizer->Add(BoldLabel(this, wxID_ANY, i18n("ratio")));
    sizer->Add(m_ratio, 1, wxEXPAND);

    sizer->Add(BoldLabel(this, wxID_ANY, i18n("last_download")));
    sizer->Add(m_lastDownload, 1, wxEXPAND);
    sizer->Add(BoldLabel(this, wxID_ANY, i18n("last_upload")));
    sizer->Add(m_lastUpload, 1, wxEXPAND);

    sizer->Add(BoldLabel(this, wxID_ANY, i18n("total_download")));
    sizer->Add(m_totalDownload, 1, wxEXPAND);
    sizer->Add(BoldLabel(this, wxID_ANY, i18n("total_upload")));
    sizer->Add(m_totalUpload, 1, wxEXPAND);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(sizer, 1, wxALL | wxEXPAND, FromDIP(5));

    this->SetSizer(mainSizer);
    this->FitInside();
    this->SetScrollRate(5, 5);
}

void TorrentDetailsOverviewPanel::Refresh(BitTorrent::TorrentHandle* torrent)
{
    auto status = torrent->Status();

    m_name->SetLabel(Utils::toStdWString(status.name));
    m_savePath->SetLabel(Utils::toStdWString(status.savePath));
    m_infoHash->SetLabel(status.infoHash);
    m_pieces->SetLabel(
        fmt::format(
            i18n("d_of_d"),
            status.pieces.count(),
            status.pieces.size()));
    m_ratio->SetLabel(
        fmt::format("{:.3f}", status.ratio));

    std::wstring activityDown = i18n("last_activity_unknown");
    std::wstring activityUp = i18n("last_activity_unknown");

    if (status.lastDownload.count() >= 0)
    {
        activityDown = status.lastDownload.count() < 2
            ? i18n("last_activity_just_now")
            : SecondsToFriendly(status.lastDownload);
    }

    if (status.lastUpload.count() >= 0)
    {
        activityUp = status.lastUpload.count() < 2
            ? i18n("last_activity_just_now")
            : SecondsToFriendly(status.lastUpload);
    }

    m_lastDownload->SetLabel(activityDown);
    m_lastUpload->SetLabel(activityUp);
    m_totalDownload->SetLabel(
        Utils::toHumanFileSize(status.allTimeDownload));
    m_totalUpload->SetLabel(
        Utils::toHumanFileSize(status.allTimeUpload));

    if (auto tf = status.torrentFile.lock())
    {
        m_comment->SetLabel(tf->comment());
        m_priv->SetLabel(
            tf->priv()
            ? i18n("yes")
            : i18n("no"));

        if (tf->total_size() != status.totalWanted)
        {
            m_size->SetLabel(
                fmt::format(
                    i18n("d_of_d"),
                    Utils::toHumanFileSize(status.totalWanted),
                    Utils::toHumanFileSize(tf->total_size())));
        }
        else
        {
            m_size->SetLabel(
                Utils::toHumanFileSize(status.totalWanted));
        }
    }

    this->Layout();
}

void TorrentDetailsOverviewPanel::Reset()
{
    m_name->SetLabel("-");
    m_infoHash->SetLabel("-");
    m_savePath->SetLabel("-");
    m_pieces->SetLabel("-");
    m_comment->SetLabel("-");
    m_size->SetLabel("-");
    m_priv->SetLabel("-");
    m_ratio->SetLabel("-");
    m_lastDownload->SetLabel("-");
    m_lastUpload->SetLabel("-");
    m_totalDownload->SetLabel("-");
    m_totalUpload->SetLabel("-");
}
