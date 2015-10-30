cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("PicoTorrent")

set(BOOST_ROOT      $ENV{BOOST_ROOT})
set(LIBTORRENT_ROOT $ENV{LIBTORRENT_ROOT})
set(OPENSSL_ROOT    $ENV{OPENSSL_ROOT})

set(PICOAPP_SOURCES
    src/app/application
    src/app/message_loop
    src/app/controllers/add_torrent_controller
    src/app/controllers/move_torrent_controller
    src/app/controllers/remove_torrent_controller
    src/app/controllers/torrent_context_menu_controller
    src/app/controllers/unhandled_exception_controller
    src/app/controllers/view_preferences_controller
)

set(PICOCOMMON_SOURCES
    src/common/environment
    src/common/string_operations
    #src/common/timer
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
    src/core/torrent_file
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
)

set(PICOUI_SOURCES
    src/ui/file_dialog_callback
    src/ui/main_window
    src/ui/open_file_dialog
    src/ui/open_torrent_dialog
    src/ui/preferences_dialog
    src/ui/remove_torrent_dialog
    src/ui/scaler
    src/ui/sort
    src/ui/task_dialog
    src/ui/torrent_context_menu
    src/ui/torrent_list_item
    src/ui/torrent_list_view
)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /MAP /OPT:REF /OPT:ICF")

include_directories(
    ${BOOST_ROOT}
    ${LIBTORRENT_ROOT}/include
    ${OPENSSL_ROOT}/inc32
    include/
)

link_directories(
    ${BOOST_ROOT}/stage/lib
    ${LIBTORRENT_ROOT}/bin/msvc-14.0/$(ConfigurationName)/address-model-64/crypto-openssl/deprecated-functions-off/link-static/threading-multi
    ${OPENSSL_ROOT}/out32
)

add_definitions(
    -D_UNICODE
    -D_WIN32
    -DBOOST_ALL_NO_LIB
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
    src/resources/app_icon.rc
    src/resources/main_menu.rc
    src/resources/preferences_dialog.rc
    src/resources/torrent_context_menu.rc
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
    debug     libboost_random-vc140-mt-gd-1_59
    optimized libboost_random-vc140-mt-1_59

    # Boost.System
    debug     libboost_system-vc140-mt-gd-1_59
    optimized libboost_system-vc140-mt-1_59

    # OpenSSL
    libeay32
    ssleay32

    # Rasterbar-libtorrent
    libtorrent
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
