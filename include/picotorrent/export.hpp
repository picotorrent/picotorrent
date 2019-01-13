#pragma once

#define PICO_CORE_API  __declspec(dllimport)
#define PICO_GEOIP_API __declspec(dllimport)
#define PICO_HTTP_API  __declspec(dllimport)

#if defined(PICO_BUILDING_CORE_API)
    #undef  PICO_CORE_API
    #define PICO_CORE_API  __declspec(dllexport)

#elif defined(PICO_BUILDING_GEOIP_API)
    #undef  PICO_GEOIP_API
    #define PICO_GEOIP_API __declspec(dllexport)

#elif defined(PICO_BUILDING_HTTP_API)
    #undef  PICO_HTTP_API
    #define PICO_HTTP_API  __declspec(dllexport)
#endif

