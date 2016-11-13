#include "Context.hpp"

using duk::Context;

Context::Context()
{
    m_ctx = duk_create_heap_default();
    m_owned = true;

    duk_push_pointer(m_ctx, this);
    duk_put_global_string(m_ctx, "\xff" "Context");
}

Context::Context(duk_context* ctx)
{
    m_ctx = ctx;
    m_owned = false;
}

Context::~Context()
{
    if (m_owned)
    {
        duk_destroy_heap(m_ctx);
    }
}

void Context::EvalString(std::string const& str)
{
    duk_eval_string(m_ctx, str.c_str());
}

Context* Context::Get(duk_context* ctx)
{
    duk_get_global_string(ctx, "\xff" "Context");
    Context* context = static_cast<Context*>(duk_get_pointer(ctx, -1));
    duk_pop(ctx);

    return context;
}

std::string Context::RequireString(int idx)
{
    return duk_require_string(m_ctx, idx);
}

void Context::SetGlobalFunction(std::string const& name, std::function<void(Context&)> const& func)
{
    duk_idx_t idx = duk_push_c_function(m_ctx, &Context::GlobalFunctionProxy, DUK_VARARGS);
    duk_push_string(m_ctx, name.c_str());
    duk_put_prop_string(m_ctx, idx, "\xff" "FunctionName");

    duk_put_global_string(m_ctx, name.c_str());

    m_globalFunctions.insert({ name, func });
}

duk_ret_t Context::GlobalFunctionProxy(duk_context* ctx)
{
    Context* context = Get(ctx);
    std::string functionName;

    duk_push_current_function(context->m_ctx);
    
    if (duk_get_prop_string(context->m_ctx, -1, "\xff" "FunctionName"))
    {
        functionName = duk_to_string(context->m_ctx, -1);
        duk_pop(context->m_ctx);
    }

    duk_pop(context->m_ctx);

    if (context->m_globalFunctions.find(functionName) != context->m_globalFunctions.end())
    {
        context->m_globalFunctions.at(functionName)(*context);
    }

    return 0;
}
