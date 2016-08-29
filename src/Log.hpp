#pragma once

#include <memory>
#include <ostream>
#include <thread>

#define LOG(level) \
    ::Log::GetInstance().CreateRecord(Log::Level::level, std::this_thread::get_id(), __FUNCTION__).GetStream()

class Log
{
public:
    enum Level
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    struct Record
    {
        Record(std::ostream& stream);
        ~Record();
        std::ostream& GetStream();

    private:
        std::ostream& m_stream;
    };

    Record CreateRecord(Level level, std::thread::id threadId, const char* function);
    static Log& GetInstance();

private:
    void Initialize();

    std::unique_ptr<std::ostream> m_stream;
    bool m_isInitialized;
};

std::ostream& operator<<(std::ostream &stream, const Log::Level level);
std::ostream& operator<<(std::ostream &stream, const std::wstring &str);

