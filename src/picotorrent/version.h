#pragma once

#include <winver.h>

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define VER_FILE_DESCRIPTION_STR    "PicoTorrent"
#define VER_FILE_VERSION            PICO_VERSION_MAJOR, PICO_VERSION_MINOR, PICO_VERSION_REVISION
#define VER_FILE_VERSION_STR        STRINGIZE(PICO_VERSION_MAJOR)        \
                                    "." STRINGIZE(PICO_VERSION_MINOR)    \
                                    "." STRINGIZE(PICO_VERSION_REVISION) \
                                    " (" STRINGIZE(PICO_GIT_COMMITISH) ")" \

#define VER_PRODUCTNAME_STR         "PicoTorrent"
#define VER_PRODUCT_VERSION         VER_FILE_VERSION
#define VER_PRODUCT_VERSION_STR     VER_FILE_VERSION_STR
#define VER_ORIGINAL_FILENAME_STR   VER_PRODUCTNAME_STR ".exe"
#define VER_INTERNAL_NAME_STR       VER_ORIGINAL_FILENAME_STR
#define VER_COPYRIGHT_STR           "Copyright (C) 2018, Viktor Elofsson and contributors"

#ifdef _DEBUG
  #define VER_VER_DEBUG             VS_FF_DEBUG
#else
  #define VER_VER_DEBUG             0
#endif

#define VER_FILEOS                  0x00040004
#define VER_FILEFLAGS               VER_VER_DEBUG
#define VER_FILETYPE                0x00000001
