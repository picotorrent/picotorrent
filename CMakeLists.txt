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

set(PICOTORRENTCORE_SOURCES
    # Misc
    src/core/version_info

    # BitTorrent components
    src/core/add_request
    src/core/is_valid_torrent_file
    src/core/hash
    src/core/peer
    src/core/session
    src/core/session_metrics
    src/core/torrent
    src/core/torrent_info
    src/core/torrent_state
    src/core/tracker
)

# Append the correct PAL file to the core sources
if(WIN32)
    list(APPEND PICOTORRENTCORE_SOURCES src/core/pal_win32)
else()
endif()

set(PICOTORRENT_SOURCES
    src/client/application
    src/client/application_initializer
    src/client/command_line

    # Configuration w/ parts
    src/client/configuration
    src/client/configuration_part
    src/client/configuration_session_part
    src/client/configuration_websocket_part

    src/client/environment
    src/client/message_loop
    src/client/string_operations
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
    src/client/logging/log
    src/client/logging/log_record
    src/client/net/http_client
    src/client/net/http_response
    src/client/net/uri

    # QR components
    src/client/qr/qr_code
    src/client/qr/qr_data
    src/client/qr/qr_util

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
    src/client/ui/property_sheets/details/options_page
    src/client/ui/property_sheets/details/overview_page
    src/client/ui/property_sheets/details/peers_page
    src/client/ui/property_sheets/details/trackers_page
    src/client/ui/property_sheets/preferences/advanced_page
    src/client/ui/property_sheets/preferences/connection_page
    src/client/ui/property_sheets/preferences/downloads_page
    src/client/ui/property_sheets/preferences/general_page
    src/client/ui/property_sheets/preferences/remote_page
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
    src/client/ui/torrent_drop_target

    # Security
    src/client/security/certificate_manager
    src/client/security/dh_params
    src/client/security/random_string_generator

    # WebSocket server
    src/client/ws/websocket_server
    src/client/ws/messages/pico_state_message
    src/client/ws/messages/torrent_added_message
    src/client/ws/messages/torrent_finished_message
    src/client/ws/messages/torrent_removed_message
    src/client/ws/messages/torrent_updated_message
    src/client/ws/serialization/torrent_serializer
)

if(WIN32)
    # Shared flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX")

    # Debug flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

    # Release flags
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")

    # Win32 libraries
    link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/PicoTorrent.Libs/bin/${PICO_ARCH}/$(Configuration)
    )

    set(PICO_LIBS
        dbghelp
        iphlpapi
        shlwapi

        # Rasterbar-libtorrent
        torrent

        # OpenSSL
        libeay32
        ssleay32
    )
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

    set(PICO_LIBS
        # Boost
        boost_chrono
        boost_random
        boost_system

        # Pthread
        pthread

        # OpenSSL
        ssl
        crypto
    )
endif()

include_directories(
    include/
    libs/websocketpp
    tools/PicoTorrent.Libs/include
)

add_definitions(
    -DBOOST_ALL_NO_LIB
    -DPICOJSON_USE_INT64
    -DTORRENT_NO_DEPRECATE
    -DTORRENT_USE_OPENSSL
)

# WIN32 definitions
if(WIN32)
    add_definitions(
        -D_SCL_SECURE_NO_WARNINGS
        -D_UNICODE
        -D_WIN32
        -D_WIN32_WINNT=0x0600
        -DNOMINMAX
        -DPICOCORE_DLL
        -DUNICODE
        -DWIN32
        -DWIN32_LEAN_AND_MEAN
    )
endif()

add_library(
    PicoTorrentCore
    SHARED
    ${PICOTORRENTCORE_SOURCES}
)

target_link_libraries(
    PicoTorrentCore
    ${PICO_LIBS}
)

if(WIN32)
    target_link_libraries(
        PicoTorrentCore

        # Boost Random
        debug boost_random-vc140-mt-gd-1_60
        optimized boost_random-vc140-mt-1_60

        # Boost System
        debug boost_system-vc140-mt-gd-1_60
        optimized boost_system-vc140-mt-1_60
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
        PicoTorrent

        PicoTorrentCore

        comctl32
        UxTheme
        winhttp
    )
endif()

