#include <picotorrent/client/logging/log.hpp>

#include <picotorrent/client/environment.hpp>
#include <picotorrent/core/pal.hpp>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <commctrl.h>
#include <dbghelp.h>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

using picotorrent::client::environment;
using picotorrent::client::logging::log;
using picotorrent::core::pal;

log::log()
{
    init();
}

log::~log()
{
}

picotorrent::client::logging::log& log::instance()
{
    static log inst;
    return inst;
}

void log::init()
{
    bool is_debug = true;

#ifdef NDEBUG
    is_debug = false;
#endif

    // Only do the following if we have a debug build
    // or passed --enable-logging on the command line
    std::wstring cmd = GetCommandLine();
    if (!is_debug && cmd.find(L"--enable-logging") == std::wstring::npos)
    {
        out_ = std::make_unique<std::ostringstream>();
        return;
    }
    out_ = std::make_unique<std::ostringstream>();

    DWORD pid = GetCurrentProcessId();

    std::string data_path = environment::get_data_path();
    std::string log_path = pal::combine_paths(data_path, "Logs");

    if (!pal::directory_exists(log_path))
    {
        pal::create_directories(log_path);
    }

    std::stringstream ss;
    ss << "PicoTorrent." << pid << ".log";
    std::string log_file = pal::combine_paths(log_path, ss.str());
    out_ = std::make_unique<std::ofstream>(log_file);

    SetUnhandledExceptionFilter(&log::on_unhandled_exception);
}

picotorrent::client::logging::log_record log::open_record(picotorrent::client::logging::log_level level, const char* functionName)
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
    return picotorrent::client::logging::log_record(*out_);
}

void log::set_unhandled_exception_callback(const std::function<void(const std::string&)> &callback)
{
    unhandled_exception_callback_ = callback;
}

std::ostream& picotorrent::client::logging::operator<<(std::ostream &stream, const picotorrent::client::logging::log_level level)
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
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;

#ifdef _WIN64
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrFrame.Offset = context->Rbp;
    frame.AddrPC.Offset = context->Rip;
    frame.AddrStack.Offset = context->Rsp;
#else
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
    frame.AddrFrame.Offset = context->Ebp;
    frame.AddrPC.Offset = context->Eip;
    frame.AddrStack.Offset = context->Esp;
#endif

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
            machineType,
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
