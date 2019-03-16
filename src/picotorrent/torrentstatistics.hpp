#include <stdint.h>

namespace pt
{
    struct TorrentStatistics
    {
        int64_t totalPayloadDownloadRate;
        int64_t totalPayloadUploadRate;
    };
}
