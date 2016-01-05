cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("PicoTorrent")

# Generate version information file
file (STRINGS "${CMAKE_SOURCE_DIR}/VERSION" VERSION)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get current architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PICO_ARCH "x64" )
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PICO_ARCH "x86")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

configure_file("${CMAKE_SOURCE_DIR}/src/common/version_info.cpp.in" "${CMAKE_SOURCE_DIR}/src/common/version_info.cpp" @ONLY)

set(BOOST_ROOT      $ENV{BOOST_ROOT})
set(LIBTORRENT_ROOT $ENV{LIBTORRENT_ROOT})
set(OPENSSL_ROOT    $ENV{OPENSSL_ROOT})

set(PICOAPP_SOURCES
    src/app/application
    src/app/command_line
    src/app/message_loop
    src/app/controllers/add_torrent_controller
    src/app/controllers/application_close_controller
    src/app/controllers/application_update_controller
    src/app/controllers/move_torrent_controller
    src/app/controllers/notifyicon_context_menu_controller
    src/app/controllers/remove_torrent_controller
    src/app/controllers/torrent_context_menu_controller
    src/app/controllers/unhandled_exception_controller
    src/app/controllers/view_preferences_controller
)

set(PICOCOMMON_SOURCES
    src/common/environment
    src/common/string_operations
    src/common/version_info
)

set(PICOCONFIG_SOURCES
    src/config/configuration
)

set(PICOCORE_SOURCES
    src/core/add_request
    src/core/is_valid_torrent_file
    src/core/hash
    src/core/session
    src/core/timer
    src/core/torrent
    src/core/torrent_info
    src/core/torrent_state
)

set(PICOFS_SOURCES
    src/filesystem/directory
    src/filesystem/file
    src/filesystem/path
)

set(PICOLOGGING_SOURCES
    src/logging/log
    src/logging/log_record
)

set(PICONET_SOURCES
    src/net/http_client
    src/net/http_request
    src/net/http_response
    src/net/uri
)

set(PICOUI_SOURCES
    src/ui/controls/control_base
    src/ui/controls/list_view
    src/ui/controls/menu
    src/ui/dialogs/dialog_base
    src/ui/dialogs/about_dialog
    src/ui/dialogs/add_torrent_dialog
    src/ui/dialogs/preferences_dialog
    src/ui/file_dialog_callback
    src/ui/main_window
    src/ui/notify_icon
    src/ui/notifyicon_context_menu
    src/ui/open_file_dialog
    src/ui/open_torrent_dialog
    src/ui/scaler
    src/ui/sort
    src/ui/task_dialog
    src/ui/torrent_context_menu
    src/ui/torrent_list_item
    src/ui/torrent_list_view
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")

include_directories(
    include/
    tools/PicoTorrent.Libs/include
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/PicoTorrent.Libs/bin/${PICO_ARCH}
)

add_definitions(
    -D_UNICODE
    -D_WIN32
    -D_WIN32_WINNT=0x0600
    -DBOOST_ALL_NO_LIB
    -DNOMINMAX
    -DPICOJSON_USE_INT64
    -DTORRENT_NO_DEPRECATE
    -DTORRENT_USE_OPENSSL
    -DUNICODE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
)

add_library(
    PicoApp
    STATIC
    ${PICOAPP_SOURCES}
)

add_library(
    PicoCommon
    STATIC
    ${PICOCOMMON_SOURCES}
)

add_library(
    PicoConfig
    STATIC
    ${PICOCONFIG_SOURCES}
)

add_library(
    PicoFileSystem
    STATIC
    ${PICOFS_SOURCES}
)

add_library(
    PicoCore
    STATIC
    ${PICOCORE_SOURCES}
)

add_library(
    PicoLogging
    STATIC
    ${PICOLOGGING_SOURCES}
)

add_library(
    PicoNet
    STATIC
    ${PICONET_SOURCES}
)

add_library(
    PicoUI
    STATIC
    ${PICOUI_SOURCES}
)

add_executable(
    PicoTorrent
    WIN32
    src/main

    # The resources belong to the application itself
    # to avoid linker issues.
    src/resources/pico.rc
)

target_link_libraries(
    PicoApp
    PicoCore
    PicoFileSystem
    PicoUI
    comctl32
)

target_link_libraries(
    PicoCore
    PicoCommon
    PicoConfig
    PicoLogging

    dbghelp
    iphlpapi

    # Boost.Random
    debug     libboost_random-vc140-mt-gd-1_60
    optimized libboost_random-vc140-mt-1_60

    # Boost.System
    debug     libboost_system-vc140-mt-gd-1_60
    optimized libboost_system-vc140-mt-1_60

    # OpenSSL
    libeay32
    ssleay32

    # Rasterbar-libtorrent
    debug     libtorrent-vc140-mt-gd
    optimized libtorrent-vc140-mt
)

target_link_libraries(
    PicoFileSystem
    shlwapi
)

target_link_libraries(
    PicoNet
    winhttp
)

target_link_libraries(
    PicoUI
    PicoNet

    UxTheme
)

target_link_libraries(
    PicoTorrent
    PicoApp
)
