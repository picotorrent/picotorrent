#include <picotorrent/core/torrent_state.hpp>

using picotorrent::core::torrent_state;

torrent_state::torrent_state(state_t state)
    : state_(state)
{
}

torrent_state::operator int() const
{
    return state_;
}

std::string torrent_state::to_string() const
{
    switch (state_)
    {
    case state_t::checking_resume_data:
        return "checking_resume_data";
    case state_t::downloading:
        return "downloading";
    case state_t::downloading_checking:
        return "downloading_checking";
    case state_t::downloading_forced:
        return "downloading_forced";
    case state_t::downloading_metadata:
        return "downloading_metadata";
    case state_t::downloading_paused:
        return "downloading_paused";
    case state_t::downloading_queued:
        return "downloading_queued";
    case state_t::downloading_stalled:
        return "downloading_stalled";
    case state_t::error:
        return "error";
    case state_t::unknown:
        return "unknown";
    case state_t::uploading:
        return "uploading";
    case state_t::uploading_checking:
        return "uploading_checking";
    case state_t::uploading_forced:
        return "uploading_forced";
    case state_t::uploading_paused:
        return "uploading_paused";
    case state_t::uploading_queued:
        return "uploading_queued";
    case state_t::uploading_stalled:
        return "uploading_stalled";
    }

    return "<error: unknown state>";
}
