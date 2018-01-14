#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <filesystem>
#include <memory>
#include <string>

namespace libtorrent
{
    struct torrent_status;
}

namespace pt
{
    class Translator;

    class Utils
    {
    public:
        static void OpenAndSelect(std::experimental::filesystem::v1::path path);
        static wxString ToHumanFileSize(int64_t bytes);
        static wxString ToReadableStatus(libtorrent::torrent_status const& ts, std::shared_ptr<Translator> translator);
        static std::wstring ToWideString(const char* buffer, int bufferSize);
    };
}
