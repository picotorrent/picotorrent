#include <picotorrent/common/logging/log_record.hpp>

using picotorrent::common::logging::log_record;

log_record::log_record(std::ostream &stream)
    : stream_(stream)
{
}

log_record::~log_record()
{
    stream_ << std::endl;
}

std::ostream& log_record::stream()
{
    return stream_;
}
