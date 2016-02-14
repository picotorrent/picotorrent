#include <picotorrent/api.hpp>

#include <picotorrent/core/string_operations.hpp>
#include <picotorrent/core/filesystem/directory.hpp>
#include <picotorrent/core/filesystem/file.hpp>
#include <picotorrent/core/filesystem/path.hpp>
#include <picotorrent/core/logging/log.hpp>
#include <shlwapi.h>

#include "duktape.h"

namespace fs = picotorrent::core::filesystem;
using picotorrent::core::to_string;

duk_ret_t requireNative(duk_context *ctx)
{
    duk_push_undefined(ctx);
    return 1;
}

class javascript_plugin : public picotorrent::plugin
{
public:
    void load()
    {
        fs::file boot = get_script_root().combine(L"bootloader.js");
        if (!boot.path().exists())
        {
            LOG(error) << "Script root does not exist, JavaScript engine disabled.";
            return;
        }

        ctx_ = duk_create_heap_default();

        // Set up requireNative to load native modules.
        duk_push_c_function(ctx_, requireNative, DUK_VARARGS);
        duk_put_global_string(ctx_, "requireNative");

        if (!ctx_)
        {
            LOG(error) << "Could not create Duktape heap.";
            return;
        }

        std::string p = to_string(boot.path().to_string());

        if (duk_peval_file(ctx_, p.c_str()) != 0)
        {
            LOG(error) << "Error when loading script: " << duk_safe_to_string(ctx_, -1);
            return;
        }
    }

    void unload()
    {
        duk_destroy_heap(ctx_);
        LOG(info) << "javascript_plugin unloading";
    }

private:
    fs::path get_script_root()
    {
        TCHAR path[MAX_PATH];

        if (IsDebuggerPresent())
        {
            GetFullPathName(L"../js", ARRAYSIZE(path), path, NULL);
        }
        else
        {
            GetFullPathName(L"./js", ARRAYSIZE(path), path, NULL);
        }

        return path;
    }

    duk_context *ctx_;
};

extern "C" __declspec(dllexport) picotorrent::plugin* create_plugin()
{
    return new javascript_plugin();
}
