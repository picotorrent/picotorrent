#pragma once

#include <memory>
#include <vector>
#include <windows.h>

namespace picotorrent
{
    class plugin;

namespace client
{
namespace plugins
{
    class plugin_host
    {
    public:
        void load_plugins();
        void unload_plugins();

    private:
        class assembly;
        std::vector<std::shared_ptr<assembly>> assemblies_;
        std::vector<std::shared_ptr<plugin>> plugins_;
    };
}
}
}
