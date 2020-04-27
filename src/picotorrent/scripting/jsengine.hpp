#pragma once

#include <filesystem>
#include <string>

#include <duktape.h>

namespace fs = std::filesystem;

namespace pt
{
namespace Scripting
{
    class JsEngine
    {
    public:
        JsEngine();
        virtual ~JsEngine();

        void Run(fs::path const& path);

        void Emit(std::string const& name);

    private:
        duk_context* m_ctx;
    };
}
}
