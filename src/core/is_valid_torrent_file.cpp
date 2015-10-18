#include <picotorrent/core/is_valid_torrent_file.hpp>

#include <libtorrent/bdecode.hpp>
#include <picotorrent/filesystem/file.hpp>
#include <picotorrent/filesystem/path.hpp>

namespace fs = picotorrent::filesystem;
namespace lt = libtorrent;

bool picotorrent::core::is_valid_torrent_file(const fs::path &path)
{
    fs::file f(path);
    std::vector<char> buf;
    f.read_all(buf);

    lt::error_code ec;
    lt::bdecode_node node;
    lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ec);

    return ec ? false : true;
}
