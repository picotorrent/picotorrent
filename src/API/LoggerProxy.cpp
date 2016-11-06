#include "LoggerProxy.hpp"

#include "../Log.hpp"

using API::LoggerProxy;

LoggerProxy::LogRecordProxy::LogRecordProxy(std::unique_ptr<Log::Record> record)
    : m_record(std::move(record))
{
}

std::ostream& LoggerProxy::LogRecordProxy::GetStream()
{
    return m_record->GetStream();
}

std::unique_ptr<ILogger::ILogRecord> LoggerProxy::OpenRecord(const char* level, std::thread::id threadId, const char* functionName)
{
    Log::Level ll;
    if (strcmp(level, "TRACE") == 0) { ll = Log::Level::Trace; }
    if (strcmp(level, "DEBUG") == 0) { ll = Log::Level::Debug; }
    if (strcmp(level, "INFO") == 0)  { ll = Log::Level::Info; }
    if (strcmp(level, "WARN") == 0)  { ll = Log::Level::Warning; }
    if (strcmp(level, "ERROR") == 0) { ll = Log::Level::Error; }

    std::unique_ptr<Log::Record> record = Log::GetInstance().CreateRecord(ll, threadId, functionName);

    return std::make_unique<LogRecordProxy>(std::move(record));
}
