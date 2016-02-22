#include <picotorrent/core/session_metrics.hpp>

#include <libtorrent/session_stats.hpp>

namespace lt = libtorrent;
using picotorrent::core::session_metrics;

session_metrics::session_metrics(const std::vector<lt::stats_metric> &metrics)
    : metrics_(metrics)
{
}

session_metrics::~session_metrics()
{
}

uint64_t session_metrics::dht_nodes()
{
    return get_metric("dht.dht_nodes");
}

void session_metrics::update(uint64_t *values, size_t size)
{
    values_.assign(values, values + size);
}

uint64_t session_metrics::get_metric(const char *name)
{
    if (values_.empty()) { return 0; }
    int idx = lt::find_metric_idx(name);
    return values_[idx];
}
