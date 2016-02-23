#pragma once

#include <picotorrent/common.hpp>
#include <vector>

namespace libtorrent
{
    struct stats_metric;
}

namespace picotorrent
{
namespace core
{
    class session;

    class session_metrics
    {
        friend class session;

    public:
        session_metrics(const std::vector<libtorrent::stats_metric> &metrics);
        ~session_metrics();

        DLL_EXPORT uint64_t dht_nodes();

    protected:
        void update(uint64_t *values, size_t len);

    private:
        uint64_t get_metric(const char *name);
        std::vector<libtorrent::stats_metric> metrics_;
        std::vector<uint64_t> values_;
    };
}
}
