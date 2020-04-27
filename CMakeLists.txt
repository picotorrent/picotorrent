cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project("PicoTorrent")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Boost_USE_STATIC_LIBS ON)
set(OPENSSL_USE_STATIC_LIBS TRUE)

if(DEFINED ENV{OPENSSL_ROOT_DIR})
    message("Using OpenSSL dir from env variable")
    set(OPENSSL_ROOT_DIR $ENV{OPENSSL_ROOT_DIR})
endif()

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

# --------- libtorrent options
option(BUILD_SHARED_LIBS    "" off)
option(deprecated-functions "" off)
# ----------------------------

add_subdirectory(vendor/libtorrent)
add_subdirectory(vendor/wx)

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

# Get current architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PICO_ARCH "x64")
else()
    set(PICO_ARCH "x86")
endif()

# Generate buildinfo file
configure_file("${CMAKE_SOURCE_DIR}/src/picotorrent/buildinfo.cpp.in" "${CMAKE_SOURCE_DIR}/src/picotorrent/buildinfo.cpp" @ONLY)

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")

add_library(
    duktape
    STATIC
    vendor/duktape/src/duktape
)

target_include_directories(
    duktape
    INTERFACE
    vendor/duktape/src
)

add_library(
    loguru
    STATIC
    vendor/loguru/loguru
)

target_compile_definitions(
    loguru
    PRIVATE
    -D_CRT_SECURE_NO_WARNINGS
)

target_include_directories(
    loguru
    INTERFACE
    vendor/loguru
)

add_library(
    sqlite
    STATIC
    src/sqlite/sqlite3
)

target_include_directories(
    sqlite
    INTERFACE
    src/sqlite
)

add_executable(
    PicoTorrent
    WIN32

    src/picotorrent/application
    src/picotorrent/buildinfo
    src/picotorrent/crashpadinitializer
    src/picotorrent/main
    src/picotorrent/persistencemanager
    src/picotorrent/resources.rc
    src/picotorrent/updatechecker

    # BitTorrent
    src/picotorrent/bittorrent/session
    src/picotorrent/bittorrent/torrenthandle

    # Core
    src/picotorrent/core/configuration
    src/picotorrent/core/database
    src/picotorrent/core/environment
    src/picotorrent/core/utils

    # Http
    src/picotorrent/http/httpclient

    # Dialogs
    src/picotorrent/ui/dialogs/aboutdialog
    src/picotorrent/ui/dialogs/addmagnetlinkdialog
    src/picotorrent/ui/dialogs/addtorrentdialog
    src/picotorrent/ui/dialogs/preferencesconnectionpage
    src/picotorrent/ui/dialogs/preferencesdialog
    src/picotorrent/ui/dialogs/preferencesdownloadspage
    src/picotorrent/ui/dialogs/preferencesgeneralpage
    src/picotorrent/ui/dialogs/preferencesproxypage

    # Models
    src/picotorrent/ui/models/filestoragemodel
    src/picotorrent/ui/models/peerlistmodel
    src/picotorrent/ui/models/torrentlistmodel
    src/picotorrent/ui/models/trackerlistmodel

    # Persistence
    src/picotorrent/ui/persistence/persistenttorrentlistview

    # Scripting
    src/picotorrent/scripting/jsengine

    # UI
    src/picotorrent/ui/mainframe
    src/picotorrent/ui/statusbar
    src/picotorrent/ui/taskbaricon
    src/picotorrent/ui/torrentcontextmenu
    src/picotorrent/ui/torrentdetailsfilespanel
    src/picotorrent/ui/torrentdetailsoverviewpanel
    src/picotorrent/ui/torrentdetailspeerspanel
    src/picotorrent/ui/torrentdetailstrackerspanel
    src/picotorrent/ui/torrentdetailsview
    src/picotorrent/ui/torrentfilelistview
    src/picotorrent/ui/torrentlistview
    src/picotorrent/ui/translator
)

target_compile_definitions(
    PicoTorrent
    PRIVATE
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
    -DZLIB_CONST
)

target_include_directories(
    PicoTorrent
    PRIVATE

    "${Boost_INCLUDE_DIRS}"

    "${CMAKE_SOURCE_DIR}/tools/UnidentifiedDeveloper.Crashpad.vc142/build/native/crashpad/include"
    "${CMAKE_SOURCE_DIR}/tools/UnidentifiedDeveloper.Crashpad.vc142/build/native/mini_chromium/include"
    "${CMAKE_SOURCE_DIR}/vendor/libtorrent/include"
)

target_link_directories(
    PicoTorrent
    PRIVATE
    "${CMAKE_SOURCE_DIR}/tools/UnidentifiedDeveloper.Crashpad.vc142/lib/native/v142/${PICO_ARCH}"
)

target_link_libraries(
    PicoTorrent

    # wxWidgets
    core base

    # Windows
    Comctl32
    crypt32
    iphlpapi
    legacy_stdio_definitions
    shlwapi
    wininet
    winhttp

    # Boost
    ${Boost_LIBRARIES}

    # Crashpad
    crashpad_client
    crashpad_util
    mini_chromium

    # Duktape
    duktape

    # loguru
    loguru

    # Rasterbar-libtorrent
    "torrent-rasterbar"

    # OpenSSL
    OpenSSL::Crypto
    OpenSSL::SSL

    sqlite
)

# Copy scripts
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/scripts"
        "$<TARGET_FILE_DIR:PicoTorrent>/scripts"
)

# Copy Crashpad handler
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/tools/UnidentifiedDeveloper.Crashpad.vc142/lib/native/v142/${PICO_ARCH}/$<CONFIG>/crashpad_handler.exe"
        $<TARGET_FILE_DIR:PicoTorrent>
)
