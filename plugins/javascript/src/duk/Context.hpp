#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "duktape.h"

namespace duk
{
    class Context
    {
    public:
        Context();
        Context(duk_context* ctx);
        ~Context();

        void EvalString(std::string const& str);
        static Context* Get(duk_context* ctx);
        std::string RequireString(int idx);
        void SetGlobalFunction(std::string const& name, std::function<void(Context&)> const& func);

    private:
        static duk_ret_t GlobalFunctionProxy(duk_context* ctx);

        duk_context* m_ctx;
        bool m_owned;
        std::map<std::string, std::function<void(Context&)>> m_globalFunctions;
    };
}
