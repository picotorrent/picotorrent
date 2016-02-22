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

struct pico_req_t
{
    int req_ref;
    int context;
    int callback_ref;
    int data_ref;
    void *data;
    HANDLE iocp;
};

struct PICOOVERLAPPED : public OVERLAPPED
{
    pico_req_t* req;
};

int pico_ref(duk_context *ctx)
{
    if (duk_is_undefined(ctx, -1))
    {
        duk_pop(ctx);
        return 0;
    }

    // Get the refs array
    duk_push_heap_stash(ctx);
    duk_get_prop_string(ctx, -1, "refs");
    duk_remove(ctx, -2);

    // refs = refs[0]
    duk_get_prop_index(ctx, -1, 0);
    int ref = duk_get_int(ctx, -1);
    duk_pop(ctx);

    // If free slot, remove it from the list
    if (ref != 0)
    {
        // refs[0] = refs[ref]
        duk_get_prop_index(ctx, -1, ref);
        duk_put_prop_index(ctx, -2, 0);
    }
    else
    {
        // ref = refs.length;
        ref = (int)duk_get_length(ctx, -1);
    }

    // swap the array and the user value in the stack
    duk_insert(ctx, -2);

    // refs[ref] = value
    duk_put_prop_index(ctx, -2, ref);

    // Remove the refs array from the stack.
    duk_pop(ctx);

    return ref;
}

void pico_unref(duk_context *ctx, int ref)
{
    if (!ref)
    {
        return;
    }

    // Get the "refs" array in the heap stash
    duk_push_heap_stash(ctx);
    duk_get_prop_string(ctx, -1, "refs");
    duk_remove(ctx, -2);

    // Insert a new link in the freelist

    // refs[ref] = refs[0]
    duk_get_prop_index(ctx, -1, 0);
    duk_put_prop_index(ctx, -2, ref);
    // refs[0] = ref
    duk_push_int(ctx, ref);
    duk_put_prop_index(ctx, -2, 0);

    duk_pop(ctx);
}

pico_req_t* pico_setup_req(duk_context* ctx, int callback_index)
{
    pico_req_t *req = static_cast<pico_req_t*>(duk_alloc(ctx, sizeof(*req)));

    duk_get_global_string(ctx, "\xff" "iocp");
    req->iocp = static_cast<HANDLE>(duk_get_pointer(ctx, -1));
    duk_pop(ctx);

    duk_push_this(ctx);
    req->context = pico_ref(ctx);
    duk_dup(ctx, -1);
    req->req_ref = pico_ref(ctx);

    if (duk_is_function(ctx, callback_index))
    {
        duk_dup(ctx, callback_index);
        req->callback_ref = pico_ref(ctx);
    }
    else
    {
        req->callback_ref = 0;
    }

    req->data = NULL;
    req->data_ref = 0;

    return req;
}

pico_req_t* pico_cleanup_req(duk_context *ctx, pico_req_t *data) {
    pico_unref(ctx, data->req_ref);
    pico_unref(ctx, data->context);
    pico_unref(ctx, data->callback_ref);
    pico_unref(ctx, data->data_ref);
    duk_free(ctx, data->data);
    duk_free(ctx, data);
    return NULL;
}

void pico_push_ref(duk_context *ctx, int ref)
{
    if (!ref)
    {
        duk_push_undefined(ctx);
        return;
    }

    // Get the "refs" array in the heap stash
    duk_push_heap_stash(ctx);
    duk_get_prop_string(ctx, -1, "refs");
    duk_remove(ctx, -2);

    duk_get_prop_index(ctx, -1, ref);

    duk_remove(ctx, -2);
}

void pico_fulfill_req(duk_context *ctx, pico_req_t *req, int nargs)
{
    if (req->callback_ref)
    {
        pico_push_ref(ctx, req->callback_ref);
        
        if (nargs)
        {
            duk_insert(ctx, -1 - nargs);
        }

        pico_push_ref(ctx, req->context);

        if (nargs)
        {
            duk_insert(ctx, -1 - nargs);
        }

        if (duk_pcall_method(ctx, nargs) != 0)
        {
            LOG(error) << "Callback error: " << duk_safe_to_string(ctx, -1);
        }

        duk_pop(ctx);
    }
    else
    {
        duk_pop_n(ctx, nargs);
    }
}

DWORD WINAPI run_fs_readdir(LPVOID lpParameter)
{
    PICOOVERLAPPED *po = new PICOOVERLAPPED();
    po->req = static_cast<pico_req_t*>(lpParameter);

    // Post to completion port
    PostQueuedCompletionStatus(
        po->req->iocp,
        0,
        NULL,
        po);

    return 0;
}

duk_ret_t fs_readdir(duk_context *ctx)
{
    pico_req_t *req = pico_setup_req(ctx, 1);

    CreateThread(
        NULL,
        0,
        run_fs_readdir,
        req,
        0,
        NULL);

    return 0;
}

duk_ret_t dukopen_fs(duk_context *ctx)
{
    const duk_function_list_entry funcs[] = {
        { "readdir", fs_readdir, 2 },
        { NULL, NULL, 0 }
    };

    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, funcs);

    return 1;
}

duk_ret_t console_log(duk_context *ctx)
{
    LOG(info) << duk_require_string(ctx, 0);
    return 0;
}

class javascript_plugin : public picotorrent::plugin
{
public:
    javascript_plugin()
    {
        io_ = CreateIoCompletionPort(
            INVALID_HANDLE_VALUE,
            NULL,
            NULL,
            NULL);
    }

    void load()
    {
        is_running_ = true;
        thread_ = CreateThread(
            NULL,
            0,
            run,
            this,
            0,
            NULL);
    }

    void unload()
    {
        LOG(info) << "Shutting down JS thread.";

        is_running_ = false;
        WaitForSingleObject(thread_, 1000);

        LOG(info) << "Unloading done.";
    }

    HANDLE iocp_handle()
    {
        return io_;
    }

private:
    static DWORD WINAPI run(LPVOID lpParameter)
    {
        javascript_plugin *jsp = reinterpret_cast<javascript_plugin*>(lpParameter);

        fs::directory scripts = get_script_root();
        if (!scripts.path().exists())
        {
            LOG(error) << "Script root does not exist, JavaScript engine disabled.";
            return 1;
        }

        duk_context *ctx = duk_create_heap_default();

        if (!ctx)
        {
            LOG(error) << "Could not create Duktape heap.";
            return 1;
        }

        // Put a reference to the plugin in as a property on the global object
        duk_push_pointer(ctx, jsp->iocp_handle());
        duk_put_global_string(ctx, "\xff" "iocp");

        // Set up ref counting
        duk_push_heap_stash(ctx);
        duk_push_array(ctx);
        duk_push_int(ctx, 0);
        duk_put_prop_index(ctx, -2, 0);
        duk_put_prop_string(ctx, -2, "refs");
        duk_pop(ctx);

        // Set up console.log
        duk_push_object(ctx);
        duk_push_c_function(ctx, console_log, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "log");
        duk_put_global_string(ctx, "console");

        // Set up the native require function
        duk_get_global_string(ctx, "Duktape");
        duk_push_c_function(ctx, require, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "modSearch");
        duk_pop(ctx);

        for (fs::path &p : scripts.get_files(L"*.js"))
        {
            std::string path = to_string(p.to_string());

            if (duk_peval_file(ctx, path.c_str()) != 0)
            {
                LOG(error) << "Error when loading script: " << duk_safe_to_string(ctx, -1);
            }
        }

        // RUn the IOCP thingy
        DWORD bytes;
        ULONG_PTR key;
        LPOVERLAPPED overlapped;

        while(jsp->is_running_)
        {
            if (GetQueuedCompletionStatus(
                jsp->io_,
                &bytes,
                &key,
                &overlapped,
                1000))
            {
                PICOOVERLAPPED *po = reinterpret_cast<PICOOVERLAPPED*>(overlapped);

                if (po != nullptr)
                {
                    // Push result
                    duk_push_undefined(ctx);
                    duk_push_string(ctx, "Hello!");

                    pico_fulfill_req(ctx, po->req, 2);
                    pico_cleanup_req(ctx, po->req);
                }
            }
        }

        duk_destroy_heap(ctx);

        return 0;
    }

    static duk_ret_t require(duk_context *ctx)
    {
        std::string id = duk_require_string(ctx, 0);

        // If id is a native module, load and return.
        if (id == "fs")
        {
            duk_push_c_function(ctx, dukopen_fs, 0);
            duk_call(ctx, 0);

            duk_put_prop_string(ctx, 3 /* module */, "exports");
        }

        return 0;
    }

    static fs::path get_script_root()
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

    bool is_running_;
    HANDLE io_;
    HANDLE thread_;
};

extern "C" __declspec(dllexport) picotorrent::plugin* create_plugin()
{
    return new javascript_plugin();
}
