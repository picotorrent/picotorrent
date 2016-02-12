#include <picotorrent/client/ui/open_torrent_dialog.hpp>

#include <picotorrent/core/is_valid_torrent_file.hpp>
#include <picotorrent/core/filesystem/path.hpp>

namespace fs = picotorrent::core::filesystem;
using picotorrent::core::is_valid_torrent_file;
using picotorrent::client::ui::open_torrent_dialog;

bool open_torrent_dialog::on_file_ok()
{
    for (fs::path &p : get_paths())
    {
        if (!is_valid_torrent_file(p))
        {
            std::wstring msg = L"The file " + p.filename().to_string() + L" is not a valid torrent file.";

            notify_error(
                L"Not a torrent",
                msg);
            return false;
        }
    }

    return true;
}
