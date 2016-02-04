#pragma once

#include <chrono>
#include <memory>
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

        tracker(const libtorrent::announce_entry &entry);
        tracker(const tracker &that);
        ~tracker();

        std::string message() const;
        std::chrono::seconds next_announce_in() const;
        int scrape_complete() const;
        status_t status() const;
        std::string url() const;

    private:
        std::shared_ptr<libtorrent::announce_entry> ae_;
    };
}
}
