#include "utils.hpp"

#include <libtorrent/torrent_status.hpp>
#include <wx/filename.h>

#include "translator.hpp"
#include "utils_win32.hpp"

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using pt::Utils;

void Utils::OpenAndSelect(fs::path path)
{
    Utils_Win32::OpenAndSelect(path);
}

wxString Utils::ToHumanFileSize(int64_t bytes)
{
    return wxFileName::GetHumanReadableSize(bytes, "-");
}

wxString Utils::ToReadableStatus(lt::torrent_status const& ts, std::shared_ptr<pt::Translator> tr)
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

    if (paused)
    {
        if (ts.errc)
        {
            return wxString::Format(
                i18n(tr, "state_error"),
                ts.errc.message().c_str());
        }

        if (seeding)
        {
            return i18n(tr, "state_uploading_paused");
        }

        return i18n(tr, "state_downloading_paused");
    }
    else
    {
        if (queued && !checking)
        {
            if (seeding)
            {
                return i18n(tr, "state_uploading_queued");
            }

            return i18n(tr, "state_downloading_queued");
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
                    return i18n(tr, "state_uploading_forced");
                }

                if (paused)
                {
                    return i18n(tr, "state_uploading_paused");
                }

                return i18n(tr, "state_uploading");
            }
            case lt::torrent_status::state_t::checking_resume_data:
            {
                return i18n(tr, "state_checking_resume_data");
            }
            case lt::torrent_status::state_t::checking_files:
            {
                return i18n(tr, "state_downloading_checking");
            }
            case lt::torrent_status::state_t::downloading_metadata:
            {
                return i18n(tr, "state_downloading_metadata");
            }
            case lt::torrent_status::state_t::downloading:
            {
                if (forced)
                {
                    return i18n(tr, "state_downloading_forced");
                }

                return i18n(tr, "state_downloading");
            }
            }
        }
    }

    return i18n(tr, "unknown");
}

std::wstring Utils::ToWideString(const char* buffer, int bufferSize)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, buffer, bufferSize, NULL, 0);
    std::wstring result(size, '\0');
    MultiByteToWideChar(CP_UTF8, 0, buffer, bufferSize, &result[0], size);
    return result;
}
