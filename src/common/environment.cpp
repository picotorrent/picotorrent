#include <picotorrent/common/environment.hpp>

#include <picotorrent/filesystem/path.hpp>
#include <shlobj.h>

namespace fs = picotorrent::filesystem;
using picotorrent::common::environment;

fs::path environment::get_special_folder(picotorrent::common::special_folder folder)
{
    PWSTR buf;

    HRESULT hr = SHGetKnownFolderPath(
        FOLDERID_Downloads,
            0,
            NULL,
            &buf);

    fs::path p(buf);
    CoTaskMemFree(buf);

    return p;
}
