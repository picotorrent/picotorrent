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

configure_file("${CMAKE_SOURCE_DIR}/src/core/version_info.cpp.in" "${CMAKE_SOURCE_DIR}/src/core/version_info.cpp" @ONLY)

set(BOOST_ROOT      $ENV{BOOST_ROOT})
set(LIBTORRENT_ROOT $ENV{LIBTORRENT_ROOT})
set(OPENSSL_ROOT    $ENV{OPENSSL_ROOT})

set(PICOTORRENTCORE_SOURCES
    # File system components
    src/core/filesystem/directory
    src/core/filesystem/file
    src/core/filesystem/path

    # Misc
    src/core/environment
    src/core/string_operations
    src/core/version_info

    # Logging components
    src/core/logging/log
    src/core/logging/log_record

    # BitTorrent components
    src/core/add_request
    src/core/configuration
    src/core/is_valid_torrent_file
    src/core/hash
    src/core/peer
    src/core/session
    src/core/session_metrics
    src/core/timer
    src/core/torrent
    src/core/torrent_info
    src/core/torrent_state
    src/core/tracker
)

set(PICOTORRENT_SOURCES
    src/client/application
    src/client/command_line
    src/client/message_loop
    src/client/controllers/add_magnet_link_controller
    src/client/controllers/add_torrent_controller
    src/client/controllers/application_close_controller
    src/client/controllers/application_update_controller
    src/client/controllers/move_torrent_controller
    src/client/controllers/notifyicon_context_menu_controller
    src/client/controllers/remove_torrent_controller
    src/client/controllers/torrent_details_controller
    src/client/controllers/torrent_context_menu_controller
    src/client/controllers/unhandled_exception_controller
    src/client/controllers/view_preferences_controller
    src/client/i18n/translator
    src/client/net/http_client
    src/client/net/http_response
    src/client/net/uri
    src/client/ui/controls/control_base
    src/client/ui/controls/list_view
    src/client/ui/controls/menu
    src/client/ui/dialogs/dialog_base
    src/client/ui/dialogs/about_dialog
    src/client/ui/dialogs/add_torrent_dialog
    src/client/ui/dialogs/add_tracker_dialog
    src/client/ui/dialogs/magnet_link_dialog
    src/client/ui/property_sheets/property_sheet_page
    src/client/ui/property_sheets/details/files_page
    src/client/ui/property_sheets/details/overview_page
    src/client/ui/property_sheets/details/peers_page
    src/client/ui/property_sheets/details/trackers_page
    src/client/ui/property_sheets/preferences/advanced_page
    src/client/ui/property_sheets/preferences/connection_page
    src/client/ui/property_sheets/preferences/downloads_page
    src/client/ui/property_sheets/preferences/general_page
    src/client/ui/file_dialog_callback
    src/client/ui/main_window
    src/client/ui/notify_icon
    src/client/ui/notifyicon_context_menu
    src/client/ui/open_file_dialog
    src/client/ui/open_torrent_dialog
    src/client/ui/scaler
    src/client/ui/sleep_manager
    src/client/ui/status_bar
    src/client/ui/task_dialog
    src/client/ui/taskbar_list
    src/client/ui/torrent_context_menu
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")

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
    PicoTorrentCore
    SHARED
    ${PICOTORRENTCORE_SOURCES}
)

add_executable(
    PicoTorrent
    WIN32
    ${PICOTORRENT_SOURCES}
    src/main

    # The resources belong to the application itself
    # to avoid linker issues.
    src/client/pico.rc
)

target_link_libraries(
    PicoTorrentCore

    dbghelp
    iphlpapi
    shlwapi

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
    PicoTorrent

    PicoTorrentCore

    comctl32
    UxTheme
    winhttp
)
