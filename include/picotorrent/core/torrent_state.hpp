#pragma once

#include <string>

namespace picotorrent
{
namespace core
{
    class torrent_state
    {
    public:
        enum state_t
        {
            unknown = -1,
            checking_resume_data,
            downloading,
            downloading_checking,
            downloading_forced,
            downloading_metadata,
            downloading_paused,
            downloading_queued,
            downloading_stalled,
            error,
            uploading,
            uploading_checking,
            uploading_forced,
            uploading_paused,
            uploading_queued,
            uploading_stalled
        };

        torrent_state(state_t state);
        operator int() const;
        std::wstring to_string() const;

    private:
        state_t state_;
    };
}
}