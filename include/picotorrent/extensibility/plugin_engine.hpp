#pragma once

#include <memory>
#include <vector>

#include <picotorrent/export.hpp>

namespace picotorrent
{
namespace extensibility
{
    class plugin_host;

    class plugin_engine
    {
    public:
        DLL_EXPORT void load_all(const std::shared_ptr<plugin_host> &host);
        DLL_EXPORT void unload_all();

    private:
        class plugin_handle;
        std::vector<std::shared_ptr<plugin_handle>> plugins_;
    };
}
}
