#include "trackerlistmodel.hpp"

#include <fmt/format.h>
#include <fmt/xchar.h>
#include <libtorrent/announce_entry.hpp>
#include <libtorrent/peer_info.hpp>

#include "../../bittorrent//torrenthandle.hpp"
#include "../../bittorrent//torrentstatus.hpp"
#include "../../core//utils.hpp"
#include "../translator.hpp"

namespace lt = libtorrent;
using pt::UI::Models::TrackerListModel;

TrackerListModel::TrackerListModel()
{
}

TrackerListModel::~TrackerListModel()
{
}

wxDataViewItemArray TrackerListModel::GetTierNodes()
{
    wxDataViewItemArray result;

    if (m_items.size() <= 3)
    {
        return result;
    }

    for (auto iter = m_items.begin() + 3; iter != m_items.end(); iter++)
    {
        result.push_back(wxDataViewItem((*iter).get()));
    }

    return result;
}

void TrackerListModel::ResetTrackers()
{
    m_dht = nullptr;
    m_lsd = nullptr;
    m_pex = nullptr;

    m_items.clear();

    this->Cleared();
}

void TrackerListModel::Update(pt::BitTorrent::TorrentHandle* torrent)
{
    if (m_dht == nullptr)
    {
        m_dht = std::make_shared<ListItem>();
        m_dht->key = "DHT";
        m_dht->tier = -1;

        m_items.insert(
            m_items.begin(),
            m_dht);

        this->ItemAdded(wxDataViewItem(), wxDataViewItem(m_dht.get()));
    }

    if (m_lsd == nullptr)
    {
        m_lsd = std::make_shared<ListItem>();
        m_lsd->key = "LSD";
        m_lsd->tier = -1;

        m_items.insert(
            m_items.begin() + 1,
            m_lsd);

        this->ItemAdded(wxDataViewItem(), wxDataViewItem(m_lsd.get()));
    }

    if (m_pex == nullptr)
    {
        m_pex = std::make_shared<ListItem>();
        m_pex->key = "PeX";
        m_pex->tier = -1;

        m_items.insert(
            m_items.begin() + 2,
            m_pex);

        this->ItemAdded(wxDataViewItem(), wxDataViewItem(m_pex.get()));
    }

    auto trackers = torrent->Trackers();

    std::vector<lt::peer_info> peers;
    torrent->GetPeerInfo(peers);

    m_dht->numLeeches = m_dht->numSeeds = 0;
    m_lsd->numLeeches = m_lsd->numSeeds = 0;
    m_pex->numLeeches = m_pex->numSeeds = 0;

    for (lt::peer_info const& peer : peers)
    {
        bool seed = static_cast<bool>(peer.flags & lt::peer_info::seed);

        if (static_cast<bool>(peer.source & lt::peer_info::dht))
        {
            if (seed) m_dht->numSeeds += 1;
            else      m_dht->numLeeches += 1;
        }
        else if (static_cast<bool>(peer.source & lt::peer_info::lsd))
        {
            if (seed) m_lsd->numSeeds += 1;
            else      m_lsd->numLeeches += 1;
        }
        else if (static_cast<bool>(peer.source & lt::peer_info::pex))
        {
            if (seed) m_pex->numSeeds += 1;
            else      m_pex->numLeeches += 1;
        }
    }

    wxDataViewItemArray staticItems;
    staticItems.push_back(wxDataViewItem(m_dht.get()));
    staticItems.push_back(wxDataViewItem(m_lsd.get()));
    staticItems.push_back(wxDataViewItem(m_pex.get()));

    this->ItemsChanged(staticItems);

    // loop through each tier (ie. skip the first three items which are static)

    for (auto tierIterator = m_items.begin() + 3; tierIterator != m_items.end();)
    {
        auto tier = (*tierIterator);

        wxDataViewItemArray deletedTrackers;

        // loop through each tracker in this tier and remove the trackers which are not present in the
        // new copy of trackers

        for (auto trackerIterator = tier->children.begin(); trackerIterator != tier->children.end();)
        {
            auto foundIterator = std::find_if(
                trackers.begin(),
                trackers.end(),
                [tierIterator, trackerIterator](lt::announce_entry const& ae)
                {
                    return ae.url == (*trackerIterator)->key
                        && ae.tier == (*tierIterator)->tier;
                });

            if (foundIterator == trackers.end())
            {
                deletedTrackers.push_back(wxDataViewItem((*trackerIterator).get()));
                trackerIterator = tier->children.erase(trackerIterator);
            }
            else
            {
                trackerIterator++;
            }
        }

        this->ItemsDeleted(
            wxDataViewItem(tier.get()),
            deletedTrackers);

        // If the tier has no children left... Remove it
        if (tier->children.empty())
        {
            this->ItemDeleted(
                wxDataViewItem(0),
                wxDataViewItem(tier.get()));

            tierIterator = m_items.erase(tierIterator);
        }
        else
        {
            tierIterator++;
        }
    }

    // Add or update new data
    for (auto iter = trackers.begin(); iter != trackers.end(); iter++)
    {
        // Check if we have a tier for this item

        auto tierIter = std::find_if(
            m_items.begin(),
            m_items.end(),
            [iter](auto const& t) { return iter->tier == t->tier; });

        if (tierIter == m_items.end())
        {
            auto newTier = std::make_shared<ListItem>();
            newTier->key = Utils::toStdString(fmt::format(i18n("tier_n"), iter->tier));
            newTier->tier = iter->tier;

            m_items.push_back(newTier);

            tierIter = m_items.end() - 1;

            this->ItemAdded(
                wxDataViewItem(0),
                wxDataViewItem(newTier.get()));
        }

        auto endpoint = std::min_element(
            iter->endpoints.begin(),
            iter->endpoints.end(),
            [](lt::announce_endpoint const& l, lt::announce_endpoint const& r)
            {
                return l.info_hashes[lt::protocol_version::V1].fails < r.info_hashes[lt::protocol_version::V1].fails;
            });

        auto trackerIter = std::find_if(
            (*tierIter)->children.begin(),
            (*tierIter)->children.end(),
            [iter](auto const& itm)
            {
                return itm->tier == iter->tier
                    && itm->key == iter->url;
            });

        if (trackerIter == (*tierIter)->children.end())
        {
            auto newTracker = std::make_shared<ListItem>();
            newTracker->key = iter->url;
            newTracker->parent = (*tierIter);
            newTracker->tier = iter->tier;

            (*tierIter)->children.push_back(newTracker);

            trackerIter = (*tierIter)->children.end() - 1;

            this->ItemAdded(
                wxDataViewItem(newTracker->parent.get()),
                wxDataViewItem(newTracker.get()));
        }

        for (lt::announce_endpoint const& ep : iter->endpoints)
        {
            lt::announce_infohash const& ah = ep.info_hashes[lt::protocol_version::V1];

            (*trackerIter)->numDownloaded = std::max((*trackerIter)->numDownloaded, ah.scrape_downloaded);
            (*trackerIter)->numLeeches = std::max((*trackerIter)->numLeeches, ah.scrape_incomplete);
            (*trackerIter)->numSeeds = std::max((*trackerIter)->numSeeds, ah.scrape_complete);
        }

        if (endpoint != iter->endpoints.end())
        {
            lt::announce_infohash const& ah = endpoint->info_hashes[lt::protocol_version::V1];

            (*trackerIter)->failLimit = iter->fail_limit;
            (*trackerIter)->fails = ah.fails;
            (*trackerIter)->nextAnnounce = std::chrono::seconds(lt::total_seconds(ah.next_announce - lt::clock_type::now()));

            if (ah.updating)
            {
                (*trackerIter)->status = ListItemStatus::updating;
            }
            else if (ah.last_error)
            {
                std::wstring error = fmt::format(
                    i18n("error_s"),
                    ah.message.empty()
                        ? Utils::toStdWString(ah.last_error.message())
                        : fmt::format(
                            L"{0} \"{1}\"",
                            Utils::toStdWString(ah.last_error.message()),
                            Utils::toStdWString(ah.message)));

                (*trackerIter)->errorMessage = error;
                (*trackerIter)->status = ListItemStatus::error;
            }
            else if (iter->verified)
            {
                (*trackerIter)->status = ListItemStatus::working;
            }
        }

        this->ItemChanged(wxDataViewItem((*trackerIter).get()));
    }
}

unsigned int TrackerListModel::GetColumnCount() const
{
    return Column::_Max;
}

wxString TrackerListModel::GetColumnType(unsigned int) const
{
    return "string";
}

void TrackerListModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
{
    wxASSERT(item.IsOk());

    ListItem* li = static_cast<ListItem*>(item.GetID());

    switch (col)
    {
    case Column::Url:
    {
        variant = Utils::toStdWString(li->key);
        break;
    }
    case Column::Status:
    {
        switch (li->status)
        {
        case ListItemStatus::error:
        {
            variant = li->errorMessage;
            break;
        }
        case ListItemStatus::unknown:
        {
            variant = "-";
            break;
        }
        case ListItemStatus::updating:
        {
            variant = i18n("updating");
            break;
        }
        case ListItemStatus::working:
        {
            variant = i18n("ok");
            break;
        }
        }
        break;
    }
    case Column::NumDownloaded:
    {
        if (li->tier < 0 || !li->children.empty())
        {
            break;
        }

        variant = li->numDownloaded >= 0
            ? std::to_wstring(li->numDownloaded)
            : i18n("not_available");
        break;
    }
    case Column::NumLeeches:
    {
        if (!li->children.empty())
        {
            break;
        }

        variant = li->numLeeches >= 0
            ? std::to_wstring(li->numLeeches)
            : i18n("not_available");
        break;
    }
    case Column::NumSeeds:
    {
        if (!li->children.empty())
        {
            break;
        }

        variant = li->numSeeds >= 0
            ? std::to_wstring(li->numSeeds)
            : i18n("not_available");
        break;
    }
    case Column::Fails:
    {
        if (li->children.size() > 0 || li->tier < 0)
        {
            variant = "";
            break;
        }

        if (li->fails == 0)
        {
            variant = "-";
            break;
        }

        if (li->failLimit == 0)
        {
            variant = std::to_string(li->fails);
            break;
        }

        variant = fmt::format(
            i18n("d_of_d"),
            li->fails,
            li->failLimit);

        break;
    }
    case Column::NextAnnounce:
    {
        if (li->children.size() > 0 || li->tier < 0)
        {
            variant = "";
            break;
        }

        if (li->status == ListItemStatus::updating
            || li->nextAnnounce.count() <= 0)
        {
            variant = "-";
            break;
        }

        std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(li->nextAnnounce);
        std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(li->nextAnnounce - hours_left);
        std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(li->nextAnnounce - hours_left - min_left);

        if (hours_left.count() <= 0)
        {
            if (min_left.count() <= 0)
            {
                variant = fmt::format(i18n("eta_s_format"), sec_left.count());
                break;
            }

            variant = fmt::format(i18n("eta_ms_format"), min_left.count(), sec_left.count());
            break;
        }

        variant = fmt::format(
            i18n("eta_hms_format"),
            hours_left.count(),
            min_left.count(),
            sec_left.count());

        break;
    }
    }
}

bool TrackerListModel::SetValue(const wxVariant&, const wxDataViewItem&, unsigned int)
{
    return false;
}

wxDataViewItem TrackerListModel::GetParent(const wxDataViewItem& item) const
{
    if (!item.IsOk())
    {
        return wxDataViewItem(0);
    }

    ListItem* node = reinterpret_cast<ListItem*>(item.GetID());

    if (!node->parent)
    {
        return wxDataViewItem(0);
    }

    return wxDataViewItem(reinterpret_cast<void*>(node->parent.get()));
}

bool TrackerListModel::IsContainer(const wxDataViewItem& item) const
{
    if (!item.IsOk())
    {
        return true;
    }

    ListItem* node = reinterpret_cast<ListItem*>(item.GetID());
    return !(node->children.empty());
}

unsigned int TrackerListModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& array) const
{
    if (!item.IsOk())
    {
        for (auto p : m_items)
        {
            array.Add(wxDataViewItem(reinterpret_cast<void*>(p.get())));
        }

        return m_items.size();
    }
    else
    {
        ListItem* node = reinterpret_cast<ListItem*>(item.GetID());

        for (auto p : node->children)
        {
            array.Add(wxDataViewItem(reinterpret_cast<void*>(p.get())));
        }

        return node->children.size();
    }
}
