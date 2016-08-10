#include "Logger.h"

#include <picotorrent/common/logging/log.hpp>

#include <msclr/marshal_cppstd.h>

using PicoTorrent::Logging::Logger;
using PicoTorrent::Logging::LogLevel;

Logger::Logger()
{
}

void Logger::Log(LogLevel level, System::String^ sourceName, System::String^ message, System::Exception^ exception)
{
    auto sb = gcnew System::Text::StringBuilder();
    sb->Append(message);
    
    if (exception != nullptr)
    {
        sb->AppendLine();
        sb->Append(exception->ToString());
    }

    std::string src = msclr::interop::marshal_as<std::string>(sourceName);
    std::string msg = msclr::interop::marshal_as<std::string>(sb->ToString());

    switch (level)
    {
    case LogLevel::Debug:
        LOG_2(debug, src.c_str()) << msg;
        break;
    case LogLevel::Error:
        LOG_2(error, src.c_str()) << msg;
        break;
    case LogLevel::Fatal:
        LOG_2(fatal, src.c_str()) << msg;
        break;
    case LogLevel::Information:
        LOG_2(info, src.c_str()) << msg;
        break;
    case LogLevel::Verbose:
        LOG_2(trace, src.c_str()) << msg;
        break;
    case LogLevel::Warning:
        LOG_2(warning, src.c_str()) << msg;
        break;
    }
}
