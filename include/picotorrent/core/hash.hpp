#pragma once

#include <memory>
#include <picotorrent/common.hpp>
#include <string>

namespace libtorrent
{
    class sha1_hash;
}

namespace picotorrent
{
namespace core
{
    class hash
    { 
    public:
        DLL_EXPORT hash(const libtorrent::sha1_hash &h);

        DLL_EXPORT std::string to_string();

    private:
        std::unique_ptr<libtorrent::sha1_hash> hash_;
    };
}
}
