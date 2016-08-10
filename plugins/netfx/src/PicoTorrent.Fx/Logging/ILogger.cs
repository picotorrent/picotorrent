using System;

namespace PicoTorrent.Logging
{
    public interface ILogger
    {
        void Log(LogLevel level, string sourceName, string message, Exception e = null);
    }
}
