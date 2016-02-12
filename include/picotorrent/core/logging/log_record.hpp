#pragma once

#include <ostream>
#include <picotorrent/common.hpp>

namespace picotorrent
{
namespace core
{
namespace logging
{
    class log_record
    {
    public:
        DLL_EXPORT log_record(std::ostream &stream);
        DLL_EXPORT ~log_record();

        DLL_EXPORT std::ostream& stream();

    private:
        std::ostream &stream_;
    };
}
}
}
