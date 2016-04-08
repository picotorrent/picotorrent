#include <picotorrent/client/ui/open_torrent_dialog.hpp>

#include <picotorrent/core/is_valid_torrent_file.hpp>

using picotorrent::core::is_valid_torrent_file;
using picotorrent::client::ui::open_torrent_dialog;

bool open_torrent_dialog::on_file_ok()
{
    for (std::string &p : get_paths())
    {
        if (!is_valid_torrent_file(p))
        {
            std::string msg = "The file " + p + " is not a valid torrent file.";

            notify_error(
                "Not a torrent",
                msg);
            return false;
        }
    }

    return true;
}
