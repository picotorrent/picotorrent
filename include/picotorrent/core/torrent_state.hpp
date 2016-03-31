#pragma once

#include <picotorrent/common.hpp>
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

        DLL_EXPORT torrent_state(state_t state);
        DLL_EXPORT operator int() const;
        DLL_EXPORT std::string to_string() const;

    private:
        state_t state_;
    };
}
}