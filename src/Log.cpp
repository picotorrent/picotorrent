#include "Log.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <windows.h>
#include <strsafe.h>

#include "Environment.hpp"
#include "StringUtils.hpp"

namespace fs = std::experimental::filesystem::v1;

Log::Record::Record(std::ostream& stream)
    : m_stream(stream)
{
}

Log::Record::~Record()
{
    m_stream << std::endl;
}

std::ostream& Log::Record::GetStream()
{
    return m_stream;
}

std::unique_ptr<Log::Record> Log::CreateRecord(Log::Level level, std::thread::id threadId, const char* function)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buf[100];

    StringCchPrintfA(buf, _ARRAYSIZE(buf), "%02d:%02d:%02d.%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    (*m_stream)
        << level
        << " - "
        << buf
        << " - "
        << threadId
        << " - "
        << function
        << " - "
        ;

    return std::make_unique<Record>(*m_stream);
}

Log& Log::GetInstance()
{
    static Log instance;
    if (!instance.m_isInitialized)
    {
        instance.Initialize();
    }
    return instance;
}

void Log::Initialize()
{
    DWORD pid = GetCurrentProcessId();

    fs::path data_path = Environment::GetDataPath();
    fs::path log_path = data_path / "Logs";

    if (!fs::exists(log_path))
    {
        fs::create_directories(log_path);
    }

    std::stringstream ss;
    ss << "PicoTorrent." << pid << ".log";
    fs::path log_file = log_path /  ss.str();

    m_stream = std::make_unique<std::ofstream>(log_file);
    m_isInitialized = true;
}

std::ostream& operator<<(std::ostream &stream, const Log::Level level)
{
    const char* levels[] =
    {
        "TRACE",
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "FATAL"
    };

    stream << levels[level];
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const std::wstring& str)
{
    stream << TS(str);
    return stream;
}
