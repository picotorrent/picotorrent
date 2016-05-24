#pragma once

#include <memory>

#include <openssl/dh.h>

namespace picotorrent
{
namespace common
{
namespace security
{
    class dh_params
    {
    public:
        static std::shared_ptr<DH> get();
    };
}
}
}
