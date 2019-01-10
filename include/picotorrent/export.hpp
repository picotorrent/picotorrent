#pragma once

#ifdef PICO_BUILDING_DLL
    #define PICO_API __declspec(dllexport)
#else
    #define PICO_API __declspec(dllimport)
#endif
