#include "Log.hpp"

#include <fstream>
#include <sstream>

#include <windows.h>
#include <strsafe.h>

#include "Environment.hpp"
#include "IO/Directory.hpp"
#include "IO/Path.hpp"
#include "StringUtils.hpp"

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

Log::Record Log::CreateRecord(Log::Level level, std::thread::id threadId, const char* function)
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

    return Record(*m_stream);
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

    std::wstring data_path = Environment::GetDataPath();
    std::wstring log_path = IO::Path::Combine(data_path, L"Logs");

    if (!IO::Directory::Exists(log_path))
    {
        IO::Directory::Create(log_path);
    }

    std::wstringstream ss;
    ss << L"PicoTorrent." << pid << L".log";
    std::wstring log_file = IO::Path::Combine(log_path, ss.str());

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
