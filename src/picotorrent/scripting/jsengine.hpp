#pragma once

#include <filesystem>

#include <ChakraCore.h>
#include <QObject>

namespace pt
{
    class ScriptedTorrentFilter;

    class JsEngine : public QObject
    {
        Q_OBJECT

    public:
        JsEngine(QObject* parent = nullptr);
        virtual ~JsEngine();

        void loadDirectory(std::experimental::filesystem::path const& path);
        void loadFile(std::experimental::filesystem::path const& path);

    signals:
        void torrentFilterAdded(ScriptedTorrentFilter* filter);

    private:
        // JS callbacks
        static JsValueRef CALLBACK addFilter(JsValueRef callee, bool isConstructCall, JsValueRef* args, unsigned short argsCount, void* callbackState);
        static JsValueRef CALLBACK translate(JsValueRef callee, bool isConstructCall, JsValueRef* args, unsigned short argsCount, void* callbackState);

        JsRuntimeHandle m_runtime;
    };
}
