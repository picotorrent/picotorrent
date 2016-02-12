#include <picotorrent/api.hpp>

#include <picotorrent/core/logging/log.hpp>

class chakra_plugin : public picotorrent::plugin
{
public:
    void load()
    {
        LOG(info) << "chakra_plugin loading";
    }

    void unload()
    {
        LOG(info) << "chakra_plugin unloading";
    }
};

extern "C" __declspec(dllexport) picotorrent::plugin* create_plugin()
{
    return new chakra_plugin();
}
