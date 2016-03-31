#include <picotorrent/core/is_valid_torrent_file.hpp>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <libtorrent/bdecode.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

#include <fstream>
#include <sstream>

namespace lt = libtorrent;

bool picotorrent::core::is_valid_torrent_file(const std::string &path)
{
    std::ifstream torrent_file(path, std::ios::binary);
    if (!torrent_file) { return false; }

    std::stringstream ss;
    ss << torrent_file.rdbuf();
    std::string &buf = ss.str();

    lt::error_code ec;
    lt::bdecode_node node;
    lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ec);

    return ec ? false : true;
}
