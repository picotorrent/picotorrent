#include <stdint.h>

namespace pt
{
namespace BitTorrent
{
    struct TorrentStatistics
    {
        int64_t totalPayloadDownloadRate;
        int64_t totalPayloadUploadRate;
        int64_t totalWanted;
        int64_t totalWantedDone;
    };
}
}
