#include <picotorrent/extensibility/plugin_engine.hpp>

#include <windows.h>

#include <picotorrent/plugin.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/common/config/configuration.hpp>
#include <picotorrent/core/pal.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/extensibility/plugin_host.hpp>

using picotorrent::plugin;
using picotorrent::common::config::configuration;
using picotorrent::common::to_wstring;
using picotorrent::core::pal;
using picotorrent::core::session;
using picotorrent::extensibility::plugin_engine;
using picotorrent::extensibility::plugin_host;

const char* plugin_func_name = "create_picotorrent_plugin";
typedef plugin*(*CREATE_PLUGIN_FUNC)(plugin_host*);

struct FreeLibraryDeleter
{
    typedef HMODULE pointer;
    void operator()(HMODULE h) { FreeLibrary(h); }
};

class plugin_engine::plugin_handle
{
public:
    plugin_handle(const std::string &path)
    {
        HMODULE mod = LoadLibrary(to_wstring(path).c_str());
        module_ = std::unique_ptr<HMODULE, FreeLibraryDeleter>(mod);
    }

    void load(const std::shared_ptr<plugin_host> &host)
    {
        if (module_ == NULL)
        {
            return;
        }

        FARPROC proc = GetProcAddress(module_.get(), plugin_func_name);

        if (proc == NULL)
        {
            return;
        }

        host_ = host;

        CREATE_PLUGIN_FUNC cpf = (CREATE_PLUGIN_FUNC)proc;

        plugin_ = std::unique_ptr<plugin>(cpf(host_.get()));
        plugin_->load();
    }

    void unload()
    {
        if (plugin_ == nullptr) { return; }
        plugin_->unload();
    }

private:
    std::unique_ptr<HMODULE, FreeLibraryDeleter> module_;
    std::unique_ptr<plugin> plugin_;
    std::shared_ptr<plugin_host> host_;
};

void plugin_engine::load_all(const std::shared_ptr<plugin_host> &host)
{
    configuration &cfg = configuration::instance();
    std::vector<std::string> search_paths = cfg.plugins()->search_paths();

    for (const std::string &path : search_paths)
    {
        // 'path' is a directory where we assume the following structure,
        // - path/plugin1
        // - path/plugin2
        // - path/plugin3
        //
        // Each directory should contain a dll with the same name. So for one plugin
        // it would be 'path/plugin1/plugin1.dll' which we should load.

        std::vector<std::string> subdirs = pal::get_directory_subdirs(path);

        for (auto &dir : subdirs)
        {
            std::string name = pal::get_file_name(dir);
            std::string plugin_dll = pal::combine_paths(dir, name + ".dll");

            auto handle = std::make_shared<plugin_handle>(plugin_dll);
            handle->load(host);

            plugins_.push_back(handle);
        }
    }
}

void plugin_engine::unload_all()
{
    for (auto &plugin : plugins_)
    {
        plugin->unload();
    }
}
