#pragma once

#include <chrono>
#include <memory>
#include <picotorrent/export.hpp>
#include <string>

namespace libtorrent
{
    struct announce_entry;
}

namespace picotorrent
{
namespace core
{
    class tracker
    {
    public:
        enum status_t
        {
            unknown,
            working,
            not_working,
            updating
        };

        DLL_EXPORT tracker(const libtorrent::announce_entry &entry);
        DLL_EXPORT tracker(const tracker &that);
        DLL_EXPORT ~tracker();

        DLL_EXPORT std::string message() const;
        DLL_EXPORT std::chrono::seconds next_announce_in() const;
        DLL_EXPORT int scrape_complete() const;
        DLL_EXPORT status_t status() const;
        DLL_EXPORT std::string url() const;

    private:
        std::shared_ptr<libtorrent::announce_entry> ae_;
    };
}
}
