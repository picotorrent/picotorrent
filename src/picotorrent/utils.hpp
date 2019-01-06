#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <filesystem>
#include <memory>
#include <string>
#include <fstream>

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
        static std::experimental::filesystem::v1::path 
            SaveResourceToFile(
                const std::string& resourceName,
                std::experimental::filesystem::v1::path path = std::experimental::filesystem::v1::temp_directory_path(),
                std::wstring resourceType = std::wstring());
        static wxString ToHumanFileSize(int64_t bytes);
        static wxString ToReadableStatus(libtorrent::torrent_status const& ts, std::shared_ptr<Translator> translator);
    };
}
