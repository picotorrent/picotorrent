cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project("PicoTorrent")

# Generate version information file
file (STRINGS "${CMAKE_SOURCE_DIR}/VERSION" VERSION)

# Build version parts
set(VERSION_TMP "${VERSION}")
string(REPLACE "." ";" VERSION_LIST ${VERSION_TMP})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_REVISION)

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
    OUTPUT_VARIABLE GIT_COMMITISH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Generate buildinfo file
configure_file("${CMAKE_SOURCE_DIR}/src/picotorrent/buildinfo.cpp.in" "${CMAKE_SOURCE_DIR}/src/picotorrent/buildinfo.cpp" @ONLY)

# Get current architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BOOST_ARCH "x64")
    set(PICO_ARCH "x64")
    set(WXWIDGETS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/tools/wxWidgets/lib/vc_x64_lib")
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BOOST_ARCH "x32")
    set(PICO_ARCH "x86")
    set(WXWIDGETS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/tools/wxWidgets/lib/vc_lib")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

# Shared flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /WX")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /WX")

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")

add_definitions(
    -D_UNICODE
    -D_WIN32
    -D_WIN32_WINNT=0x0600
    -DBOOST_ALL_NO_LIB
    -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
    -DNOMINMAX
    -DPICOJSON_USE_INT64
    -DTORRENT_NO_DEPRECATE
    -DTORRENT_USE_OPENSSL
    -DUNICODE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN

    -DPICO_GIT_COMMITISH=${GIT_COMMITISH}
    -DPICO_VERSION_MAJOR=${VERSION_MAJOR}
    -DPICO_VERSION_MINOR=${VERSION_MINOR}
    -DPICO_VERSION_REVISION=${VERSION_REVISION}
)

include_directories(
    tools/Rasterbar-libtorrent/include
    tools/wxWidgets/include
    tools/wxWidgets/include/msvc
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/Rasterbar-libtorrent/lib/${PICO_ARCH}
    ${WXWIDGETS_LIB}
)

add_library(
    breakpad
    STATIC

    src/breakpad/client_info
    src/breakpad/crash_generation_client
    src/breakpad/minidump_generator
    src/breakpad/exception_handler
    src/breakpad/crash_report_sender
    src/breakpad/guid_string
    src/breakpad/http_upload
)

add_executable(
    PicoTorrent
    WIN32

    src/picotorrent/ipc/applicationoptionsconnection
    src/picotorrent/ipc/server

    src/picotorrent/http/httpclient

    src/picotorrent/addtorrentdlg
    src/picotorrent/addtorrentproc
    src/picotorrent/advancedpage
    src/picotorrent/application
    src/picotorrent/applicationoptions
    src/picotorrent/applicationupdater
    src/picotorrent/buildinfo
    src/picotorrent/config
    src/picotorrent/config_section
    src/picotorrent/config_sessionsection
    src/picotorrent/config_uisection
    src/picotorrent/environment
    src/picotorrent/errorhandler
    src/picotorrent/connectionpage
    src/picotorrent/downloadspage
    src/picotorrent/filecontextmenu
    src/picotorrent/filespage
    src/picotorrent/filestorageviewmodel
    src/picotorrent/generalpage
    src/picotorrent/magnetlinkvalidator
    src/picotorrent/main
    src/picotorrent/mainframe
    src/picotorrent/mainmenu
    src/picotorrent/overviewpage
    src/picotorrent/peerspage
    src/picotorrent/peersviewmodel
    src/picotorrent/persistenttorrentlistview
    src/picotorrent/preferencesdlg
    src/picotorrent/proxypage
    src/picotorrent/resources.rc
    src/picotorrent/sessionloader
    src/picotorrent/sessionsettings
    src/picotorrent/sessionstate
    src/picotorrent/sessionunloader
    src/picotorrent/statusbar
    src/picotorrent/string
    src/picotorrent/taskbaricon
    src/picotorrent/torrentcontextmenu
    src/picotorrent/trackerspage
    src/picotorrent/trackersviewmodel
    src/picotorrent/translator
    src/picotorrent/torrentdetailsview
    src/picotorrent/torrentlistview
    src/picotorrent/torrentlistviewmodel
    src/picotorrent/utils
    src/picotorrent/utils_win32
)

target_link_libraries(
    PicoTorrent

    crypt32
    iphlpapi
    wininet
    winhttp

    # Breakpad
    breakpad

    # Rasterbar-libtorrent
    libtorrent

    # OpenSSL
    libssl
    libcrypto

    # Boost System
    debug libboost_system-vc141-mt-gd-${BOOST_ARCH}-1_66
    optimized libboost_system-vc141-mt-${BOOST_ARCH}-1_66
)
