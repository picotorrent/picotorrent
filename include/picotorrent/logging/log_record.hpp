#pragma once

#include <ostream>

namespace picotorrent
{
namespace logging
{
    class log_record
    {
    public:
        log_record(std::ostream &stream);
        ~log_record();

        std::ostream& stream();

    private:
        std::ostream &stream_;
    };
}
}
