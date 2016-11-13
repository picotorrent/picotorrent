#include "JavaScriptEngine.hpp"

#include "../duk/Context.hpp"

using Engine::JavaScriptEngine;

JavaScriptEngine::JavaScriptEngine()
{
	m_ctx = std::make_unique<duk::Context>();
}

void JavaScriptEngine::Load()
{
	m_ctx->SetGlobalFunction("requireNative", std::bind(&JavaScriptEngine::RequireNative, this, std::placeholders::_1));
	m_ctx->EvalString("requireNative('picotorrent');");
}

void JavaScriptEngine::RequireNative(duk::Context& context)
{
}
