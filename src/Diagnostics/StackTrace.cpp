#include "StackTrace.hpp"

#include <array>
#include <mutex>
#include <sstream>

#include <windows.h>

#pragma warning( push )
#pragma warning( disable : 4091 )
#include <dbghelp.h>
#pragma warning ( pop )

#include "StackFrame.hpp"

using Diagnostics::StackFrame;
using Diagnostics::StackTrace;

StackTrace StackTrace::Capture(int depth, void* context)
{
    static std::mutex capture_mutex;
    std::lock_guard<std::mutex> l(capture_mutex);

    StackTrace st;

    CONTEXT context_record;
    
    if (context)
    {
        context_record = *static_cast<CONTEXT*>(context);
    }
    else
    {
        RtlCaptureContext(&context_record);
    }

    size_t size = 0;
    std::array<void*, 50> stack;

    STACKFRAME64 stack_frame;
    memset(&stack_frame, 0, sizeof(stack_frame));
#if defined(_WIN64)
    int const machine_type = IMAGE_FILE_MACHINE_AMD64;
    stack_frame.AddrPC.Offset = context_record.Rip;
    stack_frame.AddrFrame.Offset = context_record.Rbp;
    stack_frame.AddrStack.Offset = context_record.Rsp;
#else
    int const machine_type = IMAGE_FILE_MACHINE_I386;
    stack_frame.AddrPC.Offset = context_record.Eip;
    stack_frame.AddrFrame.Offset = context_record.Ebp;
    stack_frame.AddrStack.Offset = context_record.Esp;
#endif
    stack_frame.AddrPC.Mode = AddrModeFlat;
    stack_frame.AddrFrame.Mode = AddrModeFlat;
    stack_frame.AddrStack.Mode = AddrModeFlat;
    while (StackWalk64(machine_type,
        GetCurrentProcess(),
        GetCurrentThread(),
        &stack_frame,
        &context_record,
        nullptr,
        &SymFunctionTableAccess64,
        &SymGetModuleBase64,
        nullptr) && size < stack.size())
    {
        stack[size++] = reinterpret_cast<void*>(stack_frame.AddrPC.Offset);
    }

    struct symbol_bundle : SYMBOL_INFO
    {
        wchar_t name[MAX_SYM_NAME];
    };

    HANDLE p = GetCurrentProcess();
    static bool sym_initialized = false;
    if (!sym_initialized)
    {
        sym_initialized = true;
        SymInitialize(p, nullptr, true);
    }

    SymRefreshModuleList(p);

    for (size_t i = 0; i < size; i++)
    {
        DWORD_PTR frame_ptr = reinterpret_cast<DWORD_PTR>(stack[i]);

        DWORD64 displacement = 0;
        symbol_bundle symbol;
        symbol.MaxNameLen = MAX_SYM_NAME;
        symbol.SizeOfStruct = sizeof(SYMBOL_INFO);
        BOOL const has_symbol = SymFromAddr(p, frame_ptr, &displacement, &symbol);

        DWORD line_displacement = 0;
        IMAGEHLP_LINE64 line = {};
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        BOOL const has_line = SymGetLineFromAddr64(GetCurrentProcess(), frame_ptr,
            &line_displacement, &line);

        std::stringstream addr;
        addr << stack[i];

        StackFrame frame;
        frame.address = addr.str();

        if (has_symbol)
        {
            char demangled_name[256];
            if (UnDecorateSymbolName(symbol.Name, demangled_name, sizeof(demangled_name), UNDNAME_NO_THROW_SIGNATURES) == 0)
            {
                demangled_name[0] = 0;
            }

            frame.displacement = displacement;
            frame.name = demangled_name;
        }

        if (has_line)
        {
            frame.fileName = line.FileName;
            frame.lineNumber = line.LineNumber;
        }

        st.frames.push_back(frame);

        if (i == depth && depth > 0)
        {
            break;
        }
    }

    return st;
}
