#pragma once

#include <picotorrent/api.hpp>

#include "../Log.hpp"

namespace API
{
    class LoggerProxy : public ILogger
    {
    public:
        struct LogRecordProxy : public ILogRecord
        {
            LogRecordProxy(std::unique_ptr<Log::Record> record);
            std::ostream& GetStream();

        private:
            std::unique_ptr<Log::Record> m_record;
        };

        std::unique_ptr<ILogRecord> OpenRecord(const char* level, std::thread::id threadId, const char* functionName);
    };
}
