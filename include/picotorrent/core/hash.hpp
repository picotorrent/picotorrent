#pragma once

#include <memory>
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
        hash(const libtorrent::sha1_hash &h);

        std::string to_string();

    private:
        std::unique_ptr<libtorrent::sha1_hash> hash_;
    };
}
}
