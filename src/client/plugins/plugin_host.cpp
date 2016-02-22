#include <picotorrent/client/plugins/plugin_host.hpp>

#include <picotorrent/api.hpp>
#include <picotorrent/core/filesystem/directory.hpp>
#include <picotorrent/core/filesystem/path.hpp>
#include <picotorrent/core/logging/log.hpp>

#include <shlwapi.h>

namespace fs = picotorrent::core::filesystem;
using picotorrent::client::plugins::plugin_host;

typedef picotorrent::plugin* (*PluginLoadFunc)();

class plugin_host::assembly
{
public:
    assembly(HMODULE mod)
        : mod_(mod)
    {
    }

    ~assembly()
    {
        if (mod_ != NULL)
        {
            FreeLibrary(mod_);
        }
    }

private:
    HMODULE mod_;
};

void plugin_host::load_plugins()
{
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);

    fs::directory d(path);

    for (fs::path &p : d.get_files(d.path().combine(L"*.dll")))
    {
        HMODULE module = LoadLibrary(p.to_string().c_str());

        if (module == NULL)
        {
            LOG(warning) << "Could not load file, error: " << GetLastError();
            continue;
        }

        PluginLoadFunc func = (PluginLoadFunc)GetProcAddress(module, "create_plugin");

        if (func == NULL)
        {
            FreeLibrary(module);
            continue;
        }

        auto ptr = std::make_shared<assembly>(module);
        auto plg = std::shared_ptr<plugin>(func());

        assemblies_.push_back(ptr);
        plugins_.push_back(plg);

        plg->load();
    }
}

void plugin_host::unload_plugins()
{
    for (const std::shared_ptr<plugin> &p : plugins_)
    {
        p->unload();
    }
}
