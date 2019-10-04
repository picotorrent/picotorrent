#include "jsengine.hpp"

#include <fstream>
#include <sstream>

#include <QString>

#include "loguru.hpp"
#include "../scriptedtorrentfilter.hpp"
#include "../translator.hpp"

namespace fs = std::filesystem;
using pt::JsEngine;

JsEngine::JsEngine(QObject* parent)
    : QObject(parent)
{
    JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &m_runtime);
}

JsEngine::~JsEngine()
{
    JsDisposeRuntime(m_runtime);
}

void JsEngine::loadDirectory(fs::path const& path)
{
    for (auto const& entry : fs::directory_iterator(path))
    {
        fs::path scriptFilePath = entry.path();

        if (scriptFilePath.extension() != ".js")
        {
            LOG_F(WARNING, "File with ignored extension in scripts path: %s", scriptFilePath.string());
            continue;
        }

        loadFile(scriptFilePath);
    }
}

void JsEngine::loadFile(fs::path const& path)
{
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();

    JsContextRef context;
    JsCreateContext(m_runtime, &context);

    JsSetCurrentContext(context);

    JsValueRef addFilterFunc;
    JsCreateFunction(&JsEngine::addFilter, this, &addFilterFunc);

    JsValueRef i18nFunc;
    JsCreateFunction(&JsEngine::translate, this, &i18nFunc);

    JsValueRef global;
    JsGetGlobalObject(&global);

    JsPropertyIdRef addFilterProperty;
    JsCreatePropertyId("addFilter", 9, &addFilterProperty);
    JsSetProperty(global, addFilterProperty, addFilterFunc, false);

    JsPropertyIdRef i18nProperty;
    JsCreatePropertyId("i18n", 4, &i18nProperty);
    JsSetProperty(global, i18nProperty, i18nFunc, false);

    JsValueRef script;
    JsCreateString(buffer.str().data(), buffer.str().size(), &script);

    JsValueRef sourceUrl;
    JsCreateString("", 0, &sourceUrl);

    unsigned sourceContext = 0;
    JsValueRef result;
    JsErrorCode error = JsRun(script, sourceContext++, sourceUrl, JsParseScriptAttributeNone, &result);

    if (error != JsNoError)
    {
        JsValueRef ex;
        JsGetAndClearException(&ex);

        JsValueRef stringEx;
        JsConvertValueToString(ex, &stringEx);

        size_t len;
        JsCopyString(stringEx, nullptr, 0, &len);

        std::string err(len, '\0');
        JsCopyString(stringEx, &err[0], err.size(), nullptr);

        LOG_F(ERROR, "Error when loading script (%s): %s", path.string().data(), err.data());
    }
}


JsValueRef JsEngine::addFilter(JsValueRef callee, bool isConstructCall, JsValueRef* args, unsigned short argsCount, void* callbackState)
{
    JsEngine* engine = static_cast<JsEngine*>(callbackState);

    size_t len;
    JsCopyString(args[2], nullptr, 0, &len);

    std::string name(len, '\0');
    JsCopyString(args[2], &name[0], name.size(), nullptr);

    JsContextRef context;
    JsGetCurrentContext(&context);

    auto filter = new ScriptedTorrentFilter(context, args[1], QString::fromStdString(name));

    emit engine->torrentFilterAdded(filter);

    JsValueRef undefined;
    JsGetUndefinedValue(&undefined);
    return undefined;
}

JsValueRef JsEngine::translate(JsValueRef callee, bool isConstructCall, JsValueRef* args, unsigned short argsCount, void* callbackState)
{
    size_t len;
    JsCopyString(args[1], nullptr, 0, &len);

    std::string key(len, '\0');
    JsCopyString(args[1], &key[0], key.size(), nullptr);

    QString translation = i18n(QString::fromStdString(key));

    JsValueRef translationValue;
    JsCreateString(translation.toStdString().data(), translation.toStdString().size(), &translationValue);

    return translationValue;
}

