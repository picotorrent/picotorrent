#include <picotorrent/core/hash.hpp>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

namespace lt = libtorrent;
using picotorrent::core::hash;

hash::hash(const lt::sha1_hash &h)
    : hash_(new lt::sha1_hash(h))
{
}

std::string hash::to_string()
{
    return lt::to_hex(hash_->to_string());
}
