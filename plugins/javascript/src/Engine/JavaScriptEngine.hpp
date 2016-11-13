#pragma once

#include <memory>

namespace duk { class Context; }

namespace Engine
{
    class JavaScriptEngine
    {
    public:
		JavaScriptEngine();
        void Load();

	private:
		void RequireNative(duk::Context& context);

		std::unique_ptr<duk::Context> m_ctx;
    };
}
