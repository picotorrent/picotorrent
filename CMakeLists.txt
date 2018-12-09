cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("PicoTorrent")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 14)
set(OPENSSL_USE_STATIC_LIBS TRUE)

find_package(OpenSSL)
find_package(Qt5Widgets CONFIG REQUIRED)

include("cmake/boost.cmake")
include("cmake/libtorrent.cmake")

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

add_executable(
    PicoTorrent
    WIN32
    src/picotorrent/addtorrentdialog
    src/picotorrent/configuration
    src/picotorrent/database
    src/picotorrent/environment
    src/picotorrent/filestorageitemmodel
    src/picotorrent/main
    src/picotorrent/mainwindow
    src/picotorrent/resources.qrc
    src/picotorrent/resources.rc
    #src/picotorrent/sessionloader
    src/picotorrent/torrentdetailswidget
    src/picotorrent/torrentlistmodel
    src/picotorrent/torrentlistwidget
    #src/picotorrent/translator
    src/picotorrent/utils
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
)

target_include_directories(
    PicoTorrent
    PRIVATE
    "${BOOST_INCLUDE_DIRS}"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/vendor/libtorrent/include"
)

target_link_libraries(
    PicoTorrent

    crypt32
    iphlpapi
    legacy_stdio_definitions
    shlwapi

    # Boost
    boost-system

    # Breakpad
    breakpad

    # sqlite
    sqlite

    # Rasterbar-libtorrent
    libtorrent-rasterbar

    # OpenSSL
    OpenSSL::Crypto
    OpenSSL::SSL

    Qt5::Widgets
)

add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Core>
        $<TARGET_FILE:Qt5::Gui>
        $<TARGET_FILE:Qt5::Widgets>
        $<TARGET_FILE_DIR:PicoTorrent>
)

add_subdirectory(plugins/torrent-details-files)
add_subdirectory(plugins/torrent-details-overview)
add_subdirectory(plugins/torrent-details-peers)
