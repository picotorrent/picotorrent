#include <picotorrent/core/env.hpp>

#include <picotorrent/filesystem/path.hpp>
#include <windows.h>
#include <shlobj.h>

namespace fs = picotorrent::filesystem;
using picotorrent::core::env;

fs::path env::get_user_downloads_directory()
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
