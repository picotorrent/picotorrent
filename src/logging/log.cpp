#include <picotorrent/logging/log.hpp>

#include <commctrl.h>
#include <dbghelp.h>
#include <fstream>
#include <sstream>
#include <windows.h>

using picotorrent::logging::log;

log::log()
    : out_(std::make_unique<std::ofstream>("PicoTorrent.log"))
{
    init();
}

log::~log()
{
}

picotorrent::logging::log& log::instance()
{
    static log inst;
    return inst;
}

void log::init()
{
    SetUnhandledExceptionFilter(
        &log::on_unhandled_exception);
}

picotorrent::logging::log_record log::open_record(picotorrent::logging::log_level level, const char* functionName)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buf[100];

    sprintf_s(buf, _ARRAYSIZE(buf), "%02d:%02d:%02d.%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    (*out_)
        << level
        << " - "
        << buf
        << " - "
        << functionName
        << " - "
        ;
    return picotorrent::logging::log_record(*out_);
}

void log::set_unhandled_exception_callback(const std::function<void(const std::string&)> &callback)
{
    unhandled_exception_callback_ = callback;
}

std::ostream& picotorrent::logging::operator<<(std::ostream &stream, const picotorrent::logging::log_level level)
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

LONG log::on_unhandled_exception(PEXCEPTION_POINTERS exceptionInfo)
{
    PCONTEXT context = exceptionInfo->ContextRecord;

    STACKFRAME frame;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context->Rbp;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrPC.Offset = context->Rip;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context->Rsp;

    std::ostringstream o;
    int n = 0;

    if (!SymInitialize(
        GetCurrentProcess(),
        NULL,
        TRUE))
    {
        LOG(error) << "SymInitialize failed with error " << GetLastError();
        return EXCEPTION_EXECUTE_HANDLER;
    }

    SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES);

    do
    {
        if (frame.AddrPC.Offset != 0)
        {
            DWORD64 displacement = 0;
            DWORD offset;

            char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
            PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen = MAX_SYM_NAME;

            IMAGEHLP_LINE line = { 0 };
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

            std::string name = "<unknown>";
            std::string ln = "";

            if (SymFromAddr(
                GetCurrentProcess(),
                frame.AddrPC.Offset,
                &displacement,
                pSymbol))
            {
                name = pSymbol->Name;
            }

            if (SymGetLineFromAddr(
                GetCurrentProcess(),
                frame.AddrPC.Offset,
                &offset,
                &line))
            {
                //ln = "(" + std::string(line.FileName) + ", line " + std::to_string(line.LineNumber) + ")";
                ln = "(line " + std::to_string(line.LineNumber) + ")";
            }

            o << "at \"" << name << "\" " << ln << std::endl;

            // Break at "WinMain" to avoid cluttering the logs.
            if (name == "WinMain")
            {
                break;
            }
        }

        if (!StackWalk(
            IMAGE_FILE_MACHINE_AMD64,
            GetCurrentProcess(),
            GetCurrentThread(),
            &frame,
            context,
            NULL,
            SymFunctionTableAccess,
            SymGetModuleBase,
            NULL))
        {
            LOG(error) << "StackWalk failed with error " << GetLastError();
            break;
        }

        if (++n > 10)
        {
            break;
        }
    } while (frame.AddrReturn.Offset != 0);

    SymCleanup(GetCurrentProcess());
    LOG(fatal) << "Unhandled exception occured" << std::endl << o.str();

    if (log::instance().unhandled_exception_callback_)
    {
        log::instance().unhandled_exception_callback_(o.str());
    }

    return EXCEPTION_EXECUTE_HANDLER;
}
