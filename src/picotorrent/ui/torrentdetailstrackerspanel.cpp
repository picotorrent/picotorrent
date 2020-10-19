#include "torrentdetailstrackerspanel.hpp"

#include <libtorrent/announce_entry.hpp>

#include <boost/log/trivial.hpp>
#include <wx/clipbrd.h>
#include <wx/dataview.h>
#include <wx/sizer.h>

#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "dialogs/addtrackerdialog.hpp"
#include "models/trackerlistmodel.hpp"
#include "translator.hpp"

using pt::UI::TorrentDetailsTrackersPanel;
using pt::UI::Models::TrackerListModel;

TorrentDetailsTrackersPanel::TorrentDetailsTrackersPanel(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id),
    m_trackersModel(new Models::TrackerListModel()),
    m_trackersView(new wxDataViewCtrl(this, wxID_ANY)),
    m_torrent(nullptr)
{
    m_trackersView->AppendTextColumn(
        i18n("url"),
        TrackerListModel::Column::Url,
        wxDATAVIEW_CELL_INERT,
        FromDIP(220));

    m_trackersView->AppendTextColumn(
        i18n("status"),
        TrackerListModel::Column::Status,
        wxDATAVIEW_CELL_INERT,
        FromDIP(160));

    m_trackersView->AppendTextColumn(
        i18n("downloaded"),
        TrackerListModel::Column::NumDownloaded,
        wxDATAVIEW_CELL_INERT,
        FromDIP(80),
        wxALIGN_RIGHT);

    m_trackersView->AppendTextColumn(
        i18n("leeches"),
        TrackerListModel::Column::NumLeeches,
        wxDATAVIEW_CELL_INERT,
        FromDIP(80),
        wxALIGN_RIGHT);

    m_trackersView->AppendTextColumn(
        i18n("seeds"),
        TrackerListModel::Column::NumSeeds,
        wxDATAVIEW_CELL_INERT,
        FromDIP(80),
        wxALIGN_RIGHT);

    m_trackersView->AppendTextColumn(
        i18n("fails"),
        TrackerListModel::Column::Fails,
        wxDATAVIEW_CELL_INERT,
        FromDIP(80),
        wxALIGN_RIGHT);

    m_trackersView->AppendTextColumn(
        i18n("next_announce"),
        TrackerListModel::Column::NextAnnounce,
        wxDATAVIEW_CELL_INERT,
        FromDIP(120),
        wxALIGN_RIGHT)->SetMinWidth(FromDIP(100));

    // Ugly hack to prevent the last "real" column from stretching.
    m_trackersView->AppendColumn(new wxDataViewColumn(wxEmptyString, new wxDataViewTextRenderer(), TrackerListModel::Column::_Max, 0));

    m_trackersView->AssociateModel(m_trackersModel);
    m_trackersModel->DecRef();

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_trackersView, 1, wxEXPAND);
    this->SetSizerAndFit(mainSizer);

    this->Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &TorrentDetailsTrackersPanel::ShowTrackerContextMenu, this);
}

void TorrentDetailsTrackersPanel::Refresh(BitTorrent::TorrentHandle* torrent)
{
    if (!torrent->IsValid())
    {
        this->Reset();
        return;
    }

    m_trackersModel->Update(torrent);

    if (m_torrent == nullptr
        || m_torrent->InfoHash() != torrent->InfoHash())
    {
        for (auto const& node : m_trackersModel->GetTierNodes())
        {
            m_trackersView->Expand(node);
        }
    }

    m_torrent = torrent;
}

void TorrentDetailsTrackersPanel::Reset()
{
    m_torrent = nullptr;
    m_trackersModel->ResetTrackers();
}

void TorrentDetailsTrackersPanel::ShowTrackerContextMenu(wxDataViewEvent& evt)
{
    if (m_torrent == nullptr)
    {
        return;
    }

    auto item = static_cast<TrackerListModel::ListItem*>(evt.GetItem().GetID());

    if (item == nullptr)
    {
        // add tracker
        wxMenu addTrackerMenu;
        addTrackerMenu.Append(ptID_CONTEXT_MENU_ADD_TRACKER, i18n("add_tracker"));
        addTrackerMenu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent const&)
            {
                Dialogs::AddTrackerDialog addDialog(this, wxID_ANY);
                
                if (addDialog.ShowModal() == wxID_OK
                    && addDialog.GetUrl().size() > 0)
                {
                    // Get max tier
                    auto trackers = m_torrent->Trackers();
                    auto maxTier = std::max_element(
                        trackers.begin(),
                        trackers.end(),
                        [](lt::announce_entry const& lhs, lt::announce_entry const& rhs)
                        { return lhs.tier < rhs.tier; });

                    lt::announce_entry ae;
                    ae.tier = maxTier != trackers.end() ? maxTier->tier + 1 : 0;
                    ae.url = addDialog.GetUrl();
                    m_torrent->AddTracker(ae);
                    m_trackersModel->Update(m_torrent);
                }
            },
            ptID_CONTEXT_MENU_ADD_TRACKER);
        PopupMenu(&addTrackerMenu);

        return;
    }

    if (item->tier < 0)
    {
        // one of the static items. do nothing
        return;
    }

    wxMenu menu;

    if (item->children.size() > 0)
    {
        menu.Append(ptID_CONTEXT_MENU_REMOVE_TIER, i18n("remove_tier"));
    }
    else
    {
        menu.Append(ptID_CONTEXT_MENU_COPY_URL, i18n("copy_url"));
        menu.Append(ptID_CONTEXT_MENU_FORCE_REANNOUNCE, i18n("force_reannounce"));
        menu.Append(ptID_CONTEXT_MENU_SCRAPE, i18n("scrape"));
        menu.AppendSeparator();
        menu.Append(ptID_CONTEXT_MENU_REMOVE_TRACKER, i18n("remove"));
    }

    menu.Bind(
        wxEVT_MENU,
        [this, item](wxCommandEvent& evt)
        {
            auto originalTrackers = m_torrent->Trackers();
            auto selectedTracker = std::find_if(
                originalTrackers.begin(),
                originalTrackers.end(),
                [&item](lt::announce_entry const& ae)
                {
                    return ae.tier == item->tier && ae.url == item->key;
                });

            if (selectedTracker == originalTrackers.end())
            {
                BOOST_LOG_TRIVIAL(warning) << "Could not find selected tracker in torrent trackers: " << item->key;
                return;
            }

            switch (evt.GetId())
            {
            case ptID_CONTEXT_MENU_COPY_URL:
            {
                if (wxTheClipboard->Open())
                {
                    wxTheClipboard->SetData(new wxTextDataObject(selectedTracker->url));
                    wxTheClipboard->Close();
                }
                break;
            }
            case ptID_CONTEXT_MENU_FORCE_REANNOUNCE:
            {
                m_torrent->ForceReannounce(
                    0,
                    static_cast<int>(std::distance(originalTrackers.begin(), selectedTracker)));
                break;
            }
            case ptID_CONTEXT_MENU_SCRAPE:
            {
                m_torrent->ScrapeTracker(
                    static_cast<int>(std::distance(originalTrackers.begin(), selectedTracker)));
                break;
            }
            case ptID_CONTEXT_MENU_REMOVE_TIER:
            {
                originalTrackers.erase(
                    std::remove_if(
                        originalTrackers.begin(),
                        originalTrackers.end(),
                        [item](auto const& ae)
                        {
                            return ae.tier == item->tier;
                        }));

                m_torrent->ReplaceTrackers(originalTrackers);

                break;
            }
            case ptID_CONTEXT_MENU_REMOVE_TRACKER:
            {
                originalTrackers.erase(
                    std::remove_if(
                        originalTrackers.begin(),
                        originalTrackers.end(),
                        [item](auto const& ae)
                        {
                            return ae.tier == item->tier && ae.url == item->key;
                        }));

                m_torrent->ReplaceTrackers(originalTrackers);

                break;
            }
            }
        });

    PopupMenu(&menu);

    m_trackersModel->Update(m_torrent);
}
