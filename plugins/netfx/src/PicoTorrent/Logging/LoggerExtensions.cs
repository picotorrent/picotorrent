using System;

namespace PicoTorrent.Logging
{
    public static class LoggerExtensions
    {
        public static void Verbose(this ILogger logger, string message, Exception exception = null)
        {
            logger.Log(LogLevel.Verbose, message, exception);
        }

        public static void Debug(this ILogger logger, string message, Exception exception = null)
        {
            logger.Log(LogLevel.Debug, message, exception);
        }

        public static void Information(this ILogger logger, string message, Exception exception = null)
        {
            logger.Log(LogLevel.Information, message, exception);
        }

        public static void Warning(this ILogger logger, string message, Exception exception = null)
        {
            logger.Log(LogLevel.Warning, message, exception);
        }

        public static void Error(this ILogger logger, string message, Exception exception = null)
        {
            logger.Log(LogLevel.Error, message, exception);
        }

        public static void Fatal(this ILogger logger, string message, Exception exception = null)
        {
            logger.Log(LogLevel.Fatal, message, exception);
        }
    }
}
