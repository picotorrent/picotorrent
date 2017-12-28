#include "utils.hpp"

#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>

#include <libtorrent/torrent_status.hpp>

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using pt::Utils;

void Utils::OpenAndSelect(fs::path path)
{
    // TODO(platform dependent)
    LPITEMIDLIST il = ILCreateFromPath(path.c_str());
    SHOpenFolderAndSelectItems(il, 0, 0, 0);
    ILFree(il);
}

std::wstring Utils::ToHumanFileSize(int64_t bytes)
{
    TCHAR buffer[1024];
    StrFormatByteSize64(bytes, buffer, ARRAYSIZE(buffer));
    return buffer;
}

std::string Utils::ToReadableStatus(lt::torrent_status const& ts)
{
    bool paused = ((ts.flags & lt::torrent_flags::paused)
        && !(ts.flags & lt::torrent_flags::auto_managed));

    bool seeding = (ts.state == lt::torrent_status::state_t::seeding
        || ts.state == lt::torrent_status::state_t::finished);

    bool queued = ((ts.flags & lt::torrent_flags::paused)
        && (ts.flags & lt::torrent_flags::auto_managed));

    bool checking = (ts.state == lt::torrent_status::state_t::checking_files
        || ts.state == lt::torrent_status::state_t::checking_resume_data);

    bool forced = (!(ts.flags & lt::torrent_flags::paused)
        && !(ts.flags & lt::torrent_flags::auto_managed));

    // TODO(translations)

    if (paused)
    {
        if (ts.errc)
        {
            return "Error";
        }

        if (seeding)
        {
            return "Seeding (paused)";
        }

        return "Downloading (paused)";
    }
    else
    {
        if (queued && !checking)
        {
            if (seeding)
            {
                return "Queued (uploading)";
            }

            return "Queued (downloading)";
        }
        else
        {
            switch (ts.state)
            {
            case lt::torrent_status::state_t::finished:
            case lt::torrent_status::state_t::seeding:
            {
                if (forced)
                {
                    return "Uploading (forced)";
                }

                if (paused)
                {
                    return "Complete";
                }

                if (ts.upload_payload_rate > 0)
                {
                    return "Uploading";
                }
                
                return "Uploading (stalled)";
            }
            case lt::torrent_status::state_t::checking_resume_data:
            {
                return "Checking resume data";
            }
            case lt::torrent_status::state_t::checking_files:
            {
                return "Downloading (checking files)";
            }
            case lt::torrent_status::state_t::downloading_metadata:
            {
                return "Downloading metadata";
            }
            case lt::torrent_status::state_t::downloading:
            {
                if (forced)
                {
                    return "Downloading (forced)";
                }

                if (ts.download_payload_rate > 0)
                {
                    return "Downloading";
                }

                return "Downloading (stalled)";
            }
            }
        }
    }

    return "Unknown status";
}

std::wstring Utils::ToWideString(const char* buffer, int bufferSize)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, buffer, bufferSize, NULL, 0);
    std::wstring result(size, '\0');
    MultiByteToWideChar(CP_UTF8, 0, buffer, bufferSize, &result[0], size);
    return result;
}
