#include "jsengine.hpp"

#include <fstream>
#include <sstream>

#include <duktape.h>
#include <loguru.hpp>

namespace fs = std::filesystem;
using pt::Scripting::JsEngine;

duk_ret_t asdf(duk_context* ctx)
{
    return 0;
}

duk_ret_t on(duk_context* ctx)
{
    /*
    first argument should be a string - ie. the event name
    second argument should be the callback function
    */

    const char* name = duk_require_string(ctx, 0);
    duk_idx_t func = duk_require_normalize_index(ctx, 1);

    duk_push_global_stash(ctx);

    if (duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("callbacks")))
    {
        if (!duk_has_prop_string(ctx, -1, name))
        {
            duk_push_array(ctx);
            duk_put_prop_string(ctx, -2, name);
        }

        if (duk_get_prop_string(ctx, -1, name))
        {
            if (duk_is_array(ctx, -1))
            {
                duk_dup(ctx, func);
                duk_put_prop_index(ctx, -2, duk_get_length(ctx, -2));

            }

            duk_pop(ctx);
        }

        duk_pop(ctx);
    }

    duk_pop(ctx);

    return 0;
}

duk_ret_t cb_resolve_module(duk_context* ctx)
{
    const char* moduleId = duk_get_string(ctx, 0);
    const char* parentId = duk_get_string(ctx, 1);

    LOG_F(INFO, "Resolving module %s", moduleId);

    duk_push_string(ctx, moduleId);
    return 1;  /*nrets*/
}

duk_ret_t cb_load_module(duk_context* ctx)
{
    const char* moduleId = duk_require_string(ctx, 0);
    /*
     *  Entry stack: [ resolved_id exports module ]
     */

     /* Arrive at the JS source code for the module somehow. */

    if (strcmp(moduleId, "i18n") == 0)
    {
        duk_push_c_function(ctx, asdf, 1);
        return 1;
    }

    if (strcmp(moduleId, "on") == 0)
    {
        duk_push_c_function(ctx, on, 2);
        return 1;
    }

    duk_error(ctx, DUK_ERR_TYPE_ERROR, "load_cb: cannot find module: %s", moduleId);
    return 0;
}

static duk_int_t duk__eval_module_source(duk_context* ctx, void* udata);
static void duk__push_module_object(duk_context* ctx, const char* id, duk_bool_t main);

static duk_bool_t duk__get_cached_module(duk_context* ctx, const char* id) {
    duk_push_global_stash(ctx);
    (void)duk_get_prop_string(ctx, -1, "\xff" "requireCache");
    if (duk_get_prop_string(ctx, -1, id)) {
        duk_remove(ctx, -2);
        duk_remove(ctx, -2);
        return 1;
    }
    else {
        duk_pop_3(ctx);
        return 0;
    }
}

/* Place a `module` object on the top of the value stack into the require cache
 * based on its `.id` property.  As a convenience to the caller, leave the
 * object on top of the value stack afterwards.
 */
static void duk__put_cached_module(duk_context* ctx) {
    /* [ ... module ] */

    duk_push_global_stash(ctx);
    (void)duk_get_prop_string(ctx, -1, "\xff" "requireCache");
    duk_dup(ctx, -3);

    /* [ ... module stash req_cache module ] */

    (void)duk_get_prop_string(ctx, -1, "id");
    duk_dup(ctx, -2);
    duk_put_prop(ctx, -4);

    duk_pop_3(ctx);  /* [ ... module ] */
}

static void duk__del_cached_module(duk_context* ctx, const char* id) {
    duk_push_global_stash(ctx);
    (void)duk_get_prop_string(ctx, -1, "\xff" "requireCache");
    duk_del_prop_string(ctx, -1, id);
    duk_pop_2(ctx);
}

static duk_ret_t duk__handle_require(duk_context* ctx) {
    /*
     *  Value stack handling here is a bit sloppy but should be correct.
     *  Call handling will clean up any extra garbage for us.
     */

    const char* id;
    const char* parent_id;
    duk_idx_t module_idx;
    duk_idx_t stash_idx;
    duk_int_t ret;

    duk_push_global_stash(ctx);
    stash_idx = duk_normalize_index(ctx, -1);

    duk_push_current_function(ctx);
    (void)duk_get_prop_string(ctx, -1, "\xff" "moduleId");
    parent_id = duk_require_string(ctx, -1);
    (void)parent_id;  /* not used directly; suppress warning */

    /* [ id stash require parent_id ] */

    id = duk_require_string(ctx, 0);

    (void)duk_get_prop_string(ctx, stash_idx, "\xff" "modResolve");
    duk_dup(ctx, 0);   /* module ID */
    duk_dup(ctx, -3);  /* parent ID */
    duk_call(ctx, 2);

    /* [ ... stash ... resolved_id ] */

    id = duk_require_string(ctx, -1);

    if (duk__get_cached_module(ctx, id)) {
        goto have_module;  /* use the cached module */
    }

    duk__push_module_object(ctx, id, 0 /*main*/);
    duk__put_cached_module(ctx);  /* module remains on stack */

    /*
     *  From here on out, we have to be careful not to throw.  If it can't be
     *  avoided, the error must be caught and the module removed from the
     *  require cache before rethrowing.  This allows the application to
     *  reattempt loading the module.
     */

    module_idx = duk_normalize_index(ctx, -1);

    /* [ ... stash ... resolved_id module ] */

    (void)duk_get_prop_string(ctx, stash_idx, "\xff" "modLoad");
    duk_dup(ctx, -3);  /* resolved ID */
    (void)duk_get_prop_string(ctx, module_idx, "exports");
    duk_dup(ctx, module_idx);
    ret = duk_pcall(ctx, 3);
    if (ret != DUK_EXEC_SUCCESS) {
        duk__del_cached_module(ctx, id);
        (void)duk_throw(ctx);  /* rethrow */
    }

    if (duk_is_string(ctx, -1)) {
        duk_int_t ret;

        /* [ ... module source ] */

        ret = duk_safe_call(ctx, duk__eval_module_source, NULL, 2, 1);

        if (ret != DUK_EXEC_SUCCESS) {
            duk__del_cached_module(ctx, id);
            (void)duk_throw(ctx);  /* rethrow */
        }
    }
    else if (duk_is_c_function(ctx, -1)) {
        return 1;
    }
    else if (duk_is_undefined(ctx, -1)) {
        duk_pop(ctx);
    }
    else {
        duk__del_cached_module(ctx, id);
        (void)duk_type_error(ctx, "invalid module load callback return value");
    }

    /* fall through */

have_module:
    /* [ ... module ] */

    (void)duk_get_prop_string(ctx, -1, "exports");
    return 1;
}

static void duk__push_require_function(duk_context* ctx, const char* id) {
    duk_push_c_function(ctx, duk__handle_require, 1);
    duk_push_string(ctx, "name");
    duk_push_string(ctx, "require");
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE);
    duk_push_string(ctx, id);
    duk_put_prop_string(ctx, -2, "\xff" "moduleId");

    /* require.cache */
    duk_push_global_stash(ctx);
    (void)duk_get_prop_string(ctx, -1, "\xff" "requireCache");
    duk_put_prop_string(ctx, -3, "cache");
    duk_pop(ctx);

    /* require.main */
    duk_push_global_stash(ctx);
    (void)duk_get_prop_string(ctx, -1, "\xff" "mainModule");
    duk_put_prop_string(ctx, -3, "main");
    duk_pop(ctx);
}

static void duk__push_module_object(duk_context* ctx, const char* id, duk_bool_t main) {
    duk_push_object(ctx);

    /* Set this as the main module, if requested */
    if (main) {
        duk_push_global_stash(ctx);
        duk_dup(ctx, -2);
        duk_put_prop_string(ctx, -2, "\xff" "mainModule");
        duk_pop(ctx);
    }

    /* Node.js uses the canonicalized filename of a module for both module.id
     * and module.filename.  We have no concept of a file system here, so just
     * use the module ID for both values.
     */
    duk_push_string(ctx, id);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, "filename");
    duk_put_prop_string(ctx, -2, "id");

    /* module.exports = {} */
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "exports");

    /* module.loaded = false */
    duk_push_false(ctx);
    duk_put_prop_string(ctx, -2, "loaded");

    /* module.require */
    duk__push_require_function(ctx, id);
    duk_put_prop_string(ctx, -2, "require");
}

static duk_int_t duk__eval_module_source(duk_context* ctx, void* udata)
{
    const char* src;

    /*
     *  Stack: [ ... module source ]
     */

    (void)udata;

    /* Wrap the module code in a function expression.  This is the simplest
     * way to implement CommonJS closure semantics and matches the behavior of
     * e.g. Node.js.
     */
    duk_push_string(ctx, "(function(exports,require,module,__filename,__dirname){");
    src = duk_require_string(ctx, -2);
    duk_push_string(ctx, (src[0] == '#' && src[1] == '!') ? "//" : "");  /* Shebang support. */
    duk_dup(ctx, -3);  /* source */
    duk_push_string(ctx, "\n})");  /* Newline allows module last line to contain a // comment. */
    duk_concat(ctx, 4);

    /* [ ... module source func_src ] */

    (void)duk_get_prop_string(ctx, -3, "filename");
    duk_compile(ctx, DUK_COMPILE_EVAL);
    duk_call(ctx, 0);

    /* [ ... module source func ] */

    /* Set name for the wrapper function. */
    duk_push_string(ctx, "name");
    duk_push_string(ctx, "main");
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_FORCE);

    /* call the function wrapper */
    (void)duk_get_prop_string(ctx, -3, "exports");   /* exports */
    (void)duk_get_prop_string(ctx, -4, "require");   /* require */
    duk_dup(ctx, -5);                                 /* module */
    (void)duk_get_prop_string(ctx, -6, "filename");  /* __filename */
    duk_push_undefined(ctx);                          /* __dirname */
    duk_call(ctx, 5);

    /* [ ... module source result(ignore) ] */

    /* module.loaded = true */
    duk_push_true(ctx);
    duk_put_prop_string(ctx, -4, "loaded");

    /* [ ... module source retval ] */

    duk_pop_2(ctx);

    /* [ ... module ] */

    return 1;
}

/* Load a module as the 'main' module. */
duk_ret_t duk_module_node_peval_main(duk_context* ctx, const char* path) {
    /*
     *  Stack: [ ... source ]
     */

    duk__push_module_object(ctx, path, 1 /*main*/);
    /* [ ... source module ] */

    duk_dup(ctx, 0);
    /* [ ... source module source ] */

    return duk_safe_call(ctx, duk__eval_module_source, NULL, 2, 1);
}

void duk_module_node_init(duk_context* ctx) {
    /*
     *  Stack: [ ... options ] => [ ... ]
     */

    duk_idx_t options_idx;

    duk_require_object_coercible(ctx, -1);  /* error before setting up requireCache */
    options_idx = duk_require_normalize_index(ctx, -1);

    /* Initialize the require cache to a fresh object. */
    duk_push_global_stash(ctx);
    duk_push_bare_object(ctx);
    duk_put_prop_string(ctx, -2, "\xff" "requireCache");
    duk_pop(ctx);

    /* Stash callbacks for later use.  User code can overwrite them later
     * on directly by accessing the global stash.
     */
    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, options_idx, "resolve");
    duk_require_function(ctx, -1);
    duk_put_prop_string(ctx, -2, "\xff" "modResolve");
    duk_get_prop_string(ctx, options_idx, "load");
    duk_require_function(ctx, -1);
    duk_put_prop_string(ctx, -2, "\xff" "modLoad");
    duk_pop(ctx);

    /* Stash main module. */
    duk_push_global_stash(ctx);
    duk_push_undefined(ctx);
    duk_put_prop_string(ctx, -2, "\xff" "mainModule");
    duk_pop(ctx);

    /* register `require` as a global function. */
    duk_push_global_object(ctx);
    duk_push_string(ctx, "require");
    duk__push_require_function(ctx, "");
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE |
        DUK_DEFPROP_SET_WRITABLE |
        DUK_DEFPROP_SET_CONFIGURABLE);
    duk_pop(ctx);

    duk_pop(ctx);  /* pop argument */
}

JsEngine::JsEngine()
    : m_ctx(duk_create_heap_default())
{
    // push stash and create an array to store our callbacks in
    duk_push_global_stash(m_ctx);
    duk_push_object(m_ctx);
    duk_put_prop_string(m_ctx, -2, DUK_HIDDEN_SYMBOL("callbacks"));
    duk_pop(m_ctx);

    duk_push_object(m_ctx);
    duk_push_c_function(m_ctx, cb_resolve_module, DUK_VARARGS);
    duk_put_prop_string(m_ctx, -2, "resolve");
    duk_push_c_function(m_ctx, cb_load_module, DUK_VARARGS);
    duk_put_prop_string(m_ctx, -2, "load");
    duk_module_node_init(m_ctx);
}

JsEngine::~JsEngine()
{
    duk_destroy_heap(m_ctx);
}

void JsEngine::Run(fs::path const& path)
{
    std::ifstream input(path, std::ios::binary);
    std::stringstream ss;
    ss << input.rdbuf();

    const char* modulePath = "C:/Users/Viktor/src/vktr/picotorrent/src/scripts/main.js";

    duk_push_string(m_ctx, ss.str().c_str());
    duk_module_node_peval_main(m_ctx, path.string().c_str());
}

void JsEngine::Emit(std::string const& name)
{
    // emit events
    duk_push_global_stash(m_ctx);

    if (duk_get_prop_string(m_ctx, -1, DUK_HIDDEN_SYMBOL("callbacks")))
    {
        if (duk_get_prop_string(m_ctx, -1, name.c_str()))
        {
            for (duk_size_t i = 0; i < duk_get_length(m_ctx, -1); i++)
            {
                duk_get_prop_index(m_ctx, -1, i);

                if (duk_pcall(m_ctx, 0) != DUK_EXEC_SUCCESS)
                {
                    // get err
                    LOG_F(ERROR, "Error when invoking callback for event '%s': %s", name.c_str(), duk_to_string(m_ctx, -1));
                }

                duk_pop(m_ctx);
            }

            duk_pop(m_ctx);
        }

        duk_pop(m_ctx);
    }

    duk_pop(m_ctx);
}
