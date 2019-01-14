#include "trackersviewmodel.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_status.hpp>

#include "translator.hpp"

namespace lt = libtorrent;
using pt::TrackersViewModel;

TrackersViewModel::TrackersViewModel(std::shared_ptr<pt::Translator> translator)
    : m_translator(translator)
{
}

void TrackersViewModel::Clear()
{
    m_data.clear();
    Reset(0);
}

void TrackersViewModel::Update(lt::torrent_status const& ts)
{
    std::vector<lt::announce_entry> trackers = ts.handle.trackers();

    // Remove old data
    for (auto it = m_data.begin(); it != m_data.end();)
    {
        auto f = std::find_if(
            trackers.begin(),
            trackers.end(),
            [it](lt::announce_entry& ae) { return ae.url == it->url; });

        if (f == trackers.end())
        {
            it = m_data.erase(it);
            RowDeleted(std::distance(m_data.begin(), it));
        }
        else
        {
            ++it;
        }
    }

    // Add or update new data

    for (auto it = trackers.begin(); it != trackers.end(); it++)
    {
        auto f = std::find_if(
            m_data.begin(),
            m_data.end(),
            [it](lt::announce_entry& ae) { return ae.url == it->url; });

        if (f == m_data.end())
        {
            m_data.push_back(*it);
            RowAppended();
        }
        else
        {
            auto distance = std::distance(m_data.begin(), f);
            m_data.at(distance) = *it;
            RowChanged(distance);
        }
    }
}

unsigned int TrackersViewModel::GetColumnCount() const
{
    return Columns::_Max;
}

wxString TrackersViewModel::GetColumnType(unsigned int col) const
{
    return "string";
}

void TrackersViewModel::GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const
{
    lt::announce_entry const& tracker = m_data.at(row);
    auto endp = std::min_element(
        tracker.endpoints.begin(),
        tracker.endpoints.end(),
        [](lt::announce_endpoint const& l, lt::announce_endpoint const& r) { return l.fails < r.fails; });

    switch (col)
    {
    case Columns::Url:
        variant = tracker.url;
        break;
    case Columns::Fails:
    {
        if (endp != tracker.endpoints.end() && endp->fails == 0)
        {
            variant = "-";
            break;
        }

        if (tracker.fail_limit == 0)
        {
            variant = wxString::Format("%d",
                (endp != tracker.endpoints.end() ? endp->fails : 0));
        }
        else
        {
            variant = wxString::Format("%d (of %d)",
                (endp != tracker.endpoints.end() ? endp->fails : 0),
                tracker.fail_limit);
        }
        break;
    }
    case Columns::Status:
        if (endp == tracker.endpoints.end())
        {
            variant = "-";
            break;
        }

        if (endp->updating)
        {
            variant = "Updating...";
            break;
        }

        if (endp->last_error)
        {
            variant = wxString::Format(
                i18n(m_translator, "error_s"),
                endp->message.empty() ? endp->last_error.message().c_str() : wxString::Format("%s \"%s\"", endp->last_error.message().c_str(), endp->message.c_str()).c_str());
            break;
        }

        if (tracker.verified)
        {
            variant = "OK";
            break;
        }

        variant = "-";
        break;
    case Columns::NextAnnounce:
        
    }
}

bool TrackersViewModel::SetValueByRow(const wxVariant &variant, unsigned row, unsigned col)
{
    return false;
}

