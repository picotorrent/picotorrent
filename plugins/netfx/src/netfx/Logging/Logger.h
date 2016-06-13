#pragma once

namespace PicoTorrent
{
namespace Logging
{
    ref class Logger : public ILogger
    {
    public:
        Logger();

        virtual void Log(LogLevel level, System::String^ message, System::Exception^ exception);
    };

}
}
