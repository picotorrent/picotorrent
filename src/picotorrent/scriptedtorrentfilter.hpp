#pragma once

#include <map>
#include <memory>

#include <ChakraCore.h>
#include <QString>

namespace pt
{
    struct TorrentStatus;

    class ScriptedTorrentFilter
    {
    public:
        ScriptedTorrentFilter(JsContextRef context, JsValueRef func, QString const& name);
        virtual ~ScriptedTorrentFilter();

        bool includes(TorrentStatus* torrent);
        QString name();

    private:
        QString m_name;
        JsContextRef m_context;
        JsValueRef m_func;
    };
}
