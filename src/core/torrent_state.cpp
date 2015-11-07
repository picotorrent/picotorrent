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

std::wstring torrent_state::to_string() const
{
    switch (state_)
    {
    case state_t::checking_resume_data:
        return L"checking_resume_data";
    case state_t::downloading:
        return L"downloading";
    case state_t::downloading_checking:
        return L"downloading_checking";
    case state_t::downloading_forced:
        return L"downloading_forced";
    case state_t::downloading_metadata:
        return L"downloading_metadata";
    case state_t::downloading_paused:
        return L"downloading_paused";
    case state_t::downloading_queued:
        return L"downloading_queued";
    case state_t::downloading_stalled:
        return L"downloading_stalled";
    case state_t::error:
        return L"error";
    case state_t::unknown:
        return L"unknown";
    case state_t::uploading:
        return L"uploading";
    case state_t::uploading_checking:
        return L"uploading_checking";
    case state_t::uploading_forced:
        return L"uploading_forced";
    case state_t::uploading_paused:
        return L"uploading_paused";
    case state_t::uploading_queued:
        return L"uploading_queued";
    case state_t::uploading_stalled:
        return L"uploading_stalled";
    }

    return L"<error: unknown state>";
}
