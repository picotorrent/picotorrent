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
    src/Configuration
    src/Configuration_Section
    src/Configuration_SessionSection
    src/Environment
    src/Main
    src/Scaler
    src/Translator
    src/VersionInformation

    src/Controllers/AddTorrentController
    src/Controllers/TorrentDetailsController
    src/Controllers/ViewPreferencesController

    src/Core/Torrent

    src/Dialogs/AboutDialog
    src/Dialogs/AddTorrentDialog
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

    src/PropertySheets/Preferences/ConnectionPage
    src/PropertySheets/Preferences/DownloadsPage
    src/PropertySheets/Preferences/GeneralPage
    src/PropertySheets/Preferences/PreferencesSheet

    src/UI/CheckBox
    src/UI/ComboBox
    src/UI/ListView
    src/UI/MainMenu
    src/UI/PeerListView
    src/UI/StatusBar
    src/UI/TextBox
    src/UI/TorrentFileListView
    src/UI/TorrentListView

    src/client/pico.rc
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
        uxtheme

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
        -DPICO_EXPORT_DLL
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

        # Boost Random
        debug boost_random-vc140-mt-gd-1_60
        optimized boost_random-vc140-mt-1_60

        # Boost System
        debug boost_system-vc140-mt-gd-1_60
        optimized boost_system-vc140-mt-1_60
    )
endif()
