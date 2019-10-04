cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project("PicoTorrent")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(OPENSSL_USE_STATIC_LIBS TRUE)

# libtorrent
set(BUILD_SHARED_LIBS OFF)
set(deprecated-functions OFF)

if(DEFINED ENV{OPENSSL_ROOT_DIR})
    message("Using OpenSSL dir from env variable")
    set(OPENSSL_ROOT_DIR $ENV{OPENSSL_ROOT_DIR})
endif()

# find_package(OpenSSL REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Svg Widgets)
find_package(Qt5WinExtras REQUIRED)

# message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
# message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

add_subdirectory(vendor/libtorrent)
add_subdirectory(vendor/zlib)

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

# Shared flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /WX")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /WX")

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")

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

target_compile_definitions(
    breakpad
    PRIVATE
    -D_UNICODE
    -D_WIN32
    -D_WIN32_WINNT=0x0600
    -DUNICODE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
)

add_library(
    sqlite
    STATIC
    src/sqlite/sqlite3
)

# MaxMind DB library
add_library(
    maxminddb
    STATIC
    vendor/libmaxminddb/src/data-pool
    vendor/libmaxminddb/src/maxminddb
)

target_include_directories(
    maxminddb
    PRIVATE
    vendor/libmaxminddb/include
    vendor/libmaxminddb/projects/VS12
)

target_link_libraries(
    maxminddb
    Ws2_32
)

add_executable(
    PicoTorrent
    WIN32

    # Core
    src/picotorrent/core/configuration
    src/picotorrent/core/database
    src/picotorrent/core/environment
    src/picotorrent/core/utils

    # GeoIP
    src/picotorrent/core/geoip/geoip
    src/picotorrent/core/geoip/gzipdecompressor
    src/picotorrent/core/geoip/maxminddatabase

    # HTTP
    src/picotorrent/core/http/httpclient
    src/picotorrent/core/http/httprequest
    src/picotorrent/core/http/httpresponse

    # Scripting
    src/picotorrent/scripting/jsengine

    src/picotorrent/aboutdialog
    src/picotorrent/addtorrentdialog
    src/picotorrent/application
    src/picotorrent/buildinfo
    src/picotorrent/connectionsectionwidget
    src/picotorrent/downloadssectionwidget
    src/picotorrent/elidedlabel
    src/picotorrent/errorhandler
    src/picotorrent/filestorageitemdelegate
    src/picotorrent/filestorageitemmodel
    src/picotorrent/generalsectionwidget
    src/picotorrent/main
    src/picotorrent/mainwindow
    src/picotorrent/preferencesdialog
    src/picotorrent/proxysectionwidget
    src/picotorrent/resources.qrc
    src/picotorrent/resources.rc
    src/picotorrent/scriptedtorrentfilter
    src/picotorrent/session
    src/picotorrent/sessionstate
    src/picotorrent/statusbar
    src/picotorrent/systemtrayicon
    src/picotorrent/textinputdialog
    src/picotorrent/torrentcontextmenu
    src/picotorrent/torrentitemdelegate
    src/picotorrent/torrenthandle
    src/picotorrent/torrentlistmodel
    src/picotorrent/torrentlistwidget
    src/picotorrent/torrentsortfilterproxymodel
    src/picotorrent/translator
    src/picotorrent/updatechecker

    src/picotorrent/models/peerlistmodel
    src/picotorrent/models/trackerslistmodel

    src/picotorrent/torrentdetails/torrentdetailswidget
    src/picotorrent/torrentdetails/torrentfileswidget
    src/picotorrent/torrentdetails/torrentoverviewwidget
    src/picotorrent/torrentdetails/torrentpeerswidget
    src/picotorrent/torrentdetails/torrenttrackerswidget
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

    # libmaxminddb
    vendor/libmaxminddb/include
    vendor/libmaxminddb/projects/VS12

    # zlib
    vendor/zlib
    "$<TARGET_FILE_DIR:zlibstatic>/../"

    "${BOOST_INCLUDE_DIRS}"
    "${CMAKE_SOURCE_DIR}/tools/Microsoft.ChakraCore.vc140/build/native/include"
    "${CMAKE_SOURCE_DIR}/vendor/libtorrent/include"
    "${CMAKE_SOURCE_DIR}/vendor/loguru"
    "${Qt5WinExtras_INCLUDE_DIRS}"
)

target_link_directories(
    PicoTorrent
    PRIVATE
    "${CMAKE_SOURCE_DIR}/tools/Microsoft.ChakraCore.vc140/lib/native/v140/${PICO_ARCH}"
)

target_link_libraries(
    PicoTorrent

    # Windows
    Comctl32
    crypt32
    iphlpapi
    legacy_stdio_definitions
    shlwapi
    wininet
    winhttp

    # Boost
    # boost-system

    # Breakpad
    breakpad

    # ChakraCore
    ChakraCore

    # loguru
    loguru

    # Rasterbar-libtorrent
    torrent-rasterbar

    # OpenSSL
    # OpenSSL::Crypto
    # OpenSSL::SSL

    maxminddb
    sqlite
    zlibstatic

    # Qt
    Qt5::Svg
    Qt5::Widgets
    Qt5::WinExtras
)


# Copy scripts
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/scripts"
        "$<TARGET_FILE_DIR:PicoTorrent>/scripts"
)

# Copy base Qt dependencies
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Core>
        $<TARGET_FILE:Qt5::Gui>
        $<TARGET_FILE:Qt5::Svg>
        $<TARGET_FILE:Qt5::Widgets>
        $<TARGET_FILE:Qt5::WinExtras>
        $<TARGET_FILE_DIR:PicoTorrent>
)

# Copy image format plugins
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::QICOPlugin>
        "$<TARGET_FILE_DIR:PicoTorrent>/imageformats/$<TARGET_FILE_NAME:Qt5::QICOPlugin>"
)

# Copy platform plugins
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin>
        "$<TARGET_FILE_DIR:PicoTorrent>/platforms/$<TARGET_FILE_NAME:Qt5::QWindowsIntegrationPlugin>"
)

# Copy style plugins
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::QWindowsVistaStylePlugin>
        "$<TARGET_FILE_DIR:PicoTorrent>/styles/$<TARGET_FILE_NAME:Qt5::QWindowsVistaStylePlugin>"
)

# Copy ChakraCore
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/tools/Microsoft.ChakraCore.vc140/lib/native/v140/${PICO_ARCH}/$<CONFIG>/ChakraCore.dll"
        $<TARGET_FILE_DIR:PicoTorrent>
)
