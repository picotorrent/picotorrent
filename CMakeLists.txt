cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
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
    OUTPUT_VARIABLE GIT_COMMITISH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Generate buildinfo file
configure_file("${CMAKE_SOURCE_DIR}/src/picotorrent/buildinfo.cpp.in" "${CMAKE_SOURCE_DIR}/src/picotorrent/buildinfo.cpp" @ONLY)

# Get current architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PICO_ARCH "x64")
    set(WXWIDGETS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/tools/wxWidgets/wxWidgets/lib/vc_x64_lib")
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PICO_ARCH "x86")
    set(WXWIDGETS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/tools/wxWidgets/wxWidgets/lib/vc_lib")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

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

include_directories(
    tools/Rasterbar-libtorrent/Rasterbar-libtorrent/include
    tools/wxWidgets/wxWidgets/include
    tools/wxWidgets/wxWidgets/include/msvc
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/Rasterbar-libtorrent/Rasterbar-libtorrent/bin/${PICO_ARCH}
    ${WXWIDGETS_LIB}
)

add_executable(
    PicoTorrent
    WIN32
    src/picotorrent/addtorrentdlg
    src/picotorrent/addtorrentproc
    src/picotorrent/application
    src/picotorrent/buildinfo
    src/picotorrent/config
    src/picotorrent/config_section
    src/picotorrent/config_sessionsection
    src/picotorrent/config_uisection
    src/picotorrent/environment
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
)

target_link_libraries(
    PicoTorrent

    # Rasterbar-libtorrent
    torrent

    # OpenSSL
    libssl
    libcrypto

    # Boost System
    debug boost_system-vc141-mt-gd-1_65_1
    optimized boost_system-vc141-mt-1_65_1
)
