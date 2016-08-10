using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;

namespace PicoTorrent.Logging
{
    public static class LoggerExtensions
    {
        [MethodImpl(MethodImplOptions.NoInlining)]
        public static void Verbose(this ILogger logger, string message, Exception exception = null)
        {
            var caller = GetCallerMethodName();
            logger.Log(LogLevel.Verbose, $"{caller}", message, exception);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public static void Debug(this ILogger logger, string message, Exception exception = null)
        {
            var caller = GetCallerMethodName();
            logger.Log(LogLevel.Debug, $"{caller}", message, exception);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public static void Information(this ILogger logger, string message, Exception exception = null)
        {
            var caller = GetCallerMethodName();
            logger.Log(LogLevel.Information, $"{caller}", message, exception);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public static void Warning(this ILogger logger, string message, Exception exception = null)
        {
            var caller = GetCallerMethodName();
            logger.Log(LogLevel.Warning, $"{caller}", message, exception);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public static void Error(this ILogger logger, string message, Exception exception = null)
        {
            var caller = GetCallerMethodName();
            logger.Log(LogLevel.Error, $"{caller}", message, exception);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public static void Fatal(this ILogger logger, string message, Exception exception = null)
        {
            var caller = GetCallerMethodName();
            logger.Log(LogLevel.Fatal, $"{caller}", message, exception);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static string GetCallerMethodName()
        {
            var frame = new StackFrame(2, false);
            var method = frame.GetMethod();
            var type = method.DeclaringType;
            var name = method.Name;

            return $"{type}.{name}";
        }
    }
}
