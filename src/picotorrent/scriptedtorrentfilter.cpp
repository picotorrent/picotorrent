#include "scriptedtorrentfilter.hpp"

#include "torrentstatus.hpp"

using pt::ScriptedTorrentFilter;

void setProperty(JsValueRef obj, QString const& name, QString const& value)
{
    JsPropertyIdRef propId;
    JsCreatePropertyId(name.toStdString().data(), name.size(), &propId);

    JsValueRef val;
    JsCreateString(value.toStdString().data(), value.size(), &val);

    JsSetProperty(obj, propId, val, false);
}

void setProperty(JsValueRef obj, QString const& name, int value)
{
    JsPropertyIdRef propId;
    JsCreatePropertyId(name.toStdString().data(), name.size(), &propId);

    JsValueRef val;
    JsIntToNumber(value, &val);

    JsSetProperty(obj, propId, val, false);
}

ScriptedTorrentFilter::ScriptedTorrentFilter(JsContextRef context, JsValueRef func, QString const& name)
    : m_context(context),
    m_func(func),
    m_name(name)
{
    JsAddRef(m_context, nullptr);
    JsAddRef(m_func, nullptr);
}

ScriptedTorrentFilter::~ScriptedTorrentFilter()
{
    JsRelease(m_context, nullptr);
    JsRelease(m_context, nullptr);
}

bool ScriptedTorrentFilter::includes(pt::TorrentStatus* status)
{
    JsSetCurrentContext(m_context);

    JsValueRef torrentObject;
    JsCreateObject(&torrentObject);

    setProperty(torrentObject, "downloadPayloadRate", status->downloadPayloadRate);
    setProperty(torrentObject, "name", status->name);
    setProperty(torrentObject, "savePath", status->savePath);
    setProperty(torrentObject, "uploadPayloadRate", status->uploadPayloadRate);

    JsValueRef args[2];
    JsValueRef undefined;
    JsGetUndefinedValue(&undefined);

    args[0] = undefined;
    args[1] = torrentObject;

    JsValueRef result;
    JsCallFunction(m_func, args, 2, &result);

    JsValueRef boolResult;
    JsConvertValueToBoolean(result, &boolResult);

    bool actualResult;
    JsBooleanToBool(boolResult, &actualResult);

    return actualResult;
}

QString ScriptedTorrentFilter::name()
{
    return m_name;
}
