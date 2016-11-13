#pragma once

#include <memory>

namespace duk { class Context; }
class IPicoTorrent;

namespace Engine
{
    class JavaScriptEngine
    {
    public:
        JavaScriptEngine(std::shared_ptr<IPicoTorrent> pico);
        void Load();

    private:
        void RequireNative(duk::Context& context);

        std::unique_ptr<duk::Context> m_ctx;
        std::shared_ptr<IPicoTorrent> m_pico;
    };
}
