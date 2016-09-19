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

configure_file("${CMAKE_SOURCE_DIR}/src/VersionInformation.cpp.in" "${CMAKE_SOURCE_DIR}/src/VersionInformation.cpp" @ONLY)

set(PICOTORRENT_SOURCES
    src/Clipboard
    src/CMainFrame
    src/CommandLine
    src/Configuration
    src/Configuration_Section
    src/Configuration_SessionSection
    src/Environment
    src/Log
    src/Main
    src/Scaler
    src/SleepManager
    src/Translator
    src/UIState
    src/UnhandledExceptionHandler
    src/VersionInformation

    src/API/LoggerProxy
    src/API/PicoTorrent
    src/API/TranslatorProxy

    src/Controllers/AddMagnetLinkController
    src/Controllers/AddTorrentController
    src/Controllers/ApplicationCloseController
    src/Controllers/NotifyIconController
    src/Controllers/RemoveTorrentsController
    src/Controllers/TorrentDetailsController
    src/Controllers/ViewPreferencesController

    src/Core/SessionLoader
    src/Core/SessionSettings
    src/Core/SessionUnloader
    src/Core/Torrent

    src/Diagnostics/StackTrace

    src/Dialogs/AboutDialog
    src/Dialogs/AddMagnetLinkDialog
    src/Dialogs/AddTorrentDialog
    src/Dialogs/AddTrackerDialog
    src/Dialogs/OpenFileDialog

    src/IO/Directory
    src/IO/File
    src/IO/Path

    src/Models/Peer
    src/Models/Torrent

    src/PropertySheets/Details/DetailsSheet
    src/PropertySheets/Details/FilesPage
    src/PropertySheets/Details/OptionsPage
    src/PropertySheets/Details/OverviewPage
    src/PropertySheets/Details/PeersPage
    src/PropertySheets/Details/TrackersPage

    src/PropertySheets/Preferences/ConnectionPage
    src/PropertySheets/Preferences/DownloadsPage
    src/PropertySheets/Preferences/GeneralPage
    src/PropertySheets/Preferences/PreferencesSheet

    src/UI/CheckBox
    src/UI/ComboBox
    src/UI/ListView
    src/UI/NotifyIcon
    src/UI/MainMenu
    src/UI/PeerListView
    src/UI/StatusBar
    src/UI/Taskbar
    src/UI/TextBox
    src/UI/TorrentFileListView
    src/UI/TorrentListView
    src/UI/TrackerListView

    src/resources.rc
)

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

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/PicoTorrent.Libs/bin/${PICO_ARCH}/$(Configuration)
)

set(PICO_LIBS
    dbghelp
    iphlpapi
    shlwapi
    uxtheme

    # Rasterbar-libtorrent
    torrent

    # OpenSSL
    libeay32
    ssleay32
)

include_directories(
    include/
    libs/websocketpp
    tools/PicoTorrent.Libs/include
)

add_definitions(
    -D_SCL_SECURE_NO_WARNINGS
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

add_executable(
    PicoTorrent
    WIN32
    ${PICOTORRENT_SOURCES}
)

target_link_libraries(
    PicoTorrent

    ${PICO_LIBS}

    # Boost System
    debug boost_system-vc140-mt-gd-1_61
    optimized boost_system-vc140-mt-1_61
)

# Include plugins
include(plugins/updatechecker/CMakeLists.txt)
