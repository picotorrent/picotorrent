#include <picotorrent/core/tracker.hpp>

#include <libtorrent/announce_entry.hpp>

namespace lt = libtorrent;
using picotorrent::core::tracker;

tracker::tracker(const lt::announce_entry &entry)
    : ae_(std::make_shared<lt::announce_entry>(entry))
{
}

tracker::tracker(const tracker &that)
    : ae_(that.ae_)
{
}

tracker::~tracker()
{
}

std::string tracker::message() const
{
    return ae_->message;
}

int tracker::scrape_complete() const
{
    return ae_->scrape_complete;
}

tracker::status_t tracker::status() const
{
    if (ae_->verified && ae_->is_working())
    {
        return status_t::working;
    }
    else if ((ae_->fails == 0) && ae_->updating)
    {
        return status_t::updating;
    }
    else if (ae_->fails == 0)
    {
        return status_t::unknown;
    }

    return status_t::not_working;
}

std::string tracker::url() const
{
    return ae_->url;
}
