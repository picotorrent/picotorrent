#pragma once

#ifdef PICO_EXPORT_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT
#endif
