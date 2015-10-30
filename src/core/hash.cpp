#include <picotorrent/core/hash.hpp>

#include <libtorrent/sha1_hash.hpp>

namespace lt = libtorrent;
using picotorrent::core::hash;

hash::hash(const lt::sha1_hash &h)
    : hash_(std::make_unique<lt::sha1_hash>(h))
{
}

std::string hash::to_string()
{
    return lt::to_hex(hash_->to_string());
}
