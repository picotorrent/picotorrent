cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("PicoTorrent")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 14)
set(OPENSSL_USE_STATIC_LIBS TRUE)

find_package(OpenSSL)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Svg Widgets)

include("cmake/boost.cmake")
include("cmake/libtorrent.cmake")
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

# PicoTorrent GeoIP library
add_library(
    PicoTorrent.GeoIP
    SHARED
    src/picotorrent-geoip/geoip
    src/picotorrent-geoip/gzipdecompressor
    src/picotorrent-geoip/maxminddatabase

    # Headers (for MOC Q_OBJECT detection)
    include/picotorrent/geoip/geoip
)

target_compile_definitions(
    PicoTorrent.GeoIP
    PRIVATE
    -DPICO_BUILDING_GEOIP_API
    -DZLIB_CONST
)

target_include_directories(
    PicoTorrent.GeoIP
    PRIVATE
    include/

    # libmaxminddb
    vendor/libmaxminddb/include
    vendor/libmaxminddb/projects/VS12

    # zlib
    vendor/zlib
    "$<TARGET_FILE_DIR:zlibstatic>/../"
)

target_link_libraries(
    PicoTorrent.GeoIP
    PRIVATE

    maxminddb
    zlibstatic

    PicoTorrent.Core
    PicoTorrent.Http

    Qt5::Core
)

# PicoTorrent HTTP library
add_library(
    PicoTorrent.Http
    SHARED
    src/picotorrent-http/httpclient
    src/picotorrent-http/httprequest
    src/picotorrent-http/httpresponse

    # Headers (for MOC Q_OBJECT detection)
    include/picotorrent/http/httprequest
    include/picotorrent/http/httpresponse
)

target_compile_definitions(
    PicoTorrent.Http
    PRIVATE
    -DPICO_BUILDING_HTTP_API
)

target_include_directories(
    PicoTorrent.Http
    PRIVATE
    include/
)

target_link_libraries(
    PicoTorrent.Http
    Qt5::Core
    winhttp
)

add_library(
    PicoTorrent.Core
    SHARED
    src/picotorrent-core/configuration
    src/picotorrent-core/database
    src/picotorrent-core/environment
    src/picotorrent-core/utils
)

target_compile_definitions(
    PicoTorrent.Core
    PRIVATE
    -DPICO_BUILDING_CORE_API
    -D_UNICODE
    -D_WIN32
    -D_WIN32_WINNT=0x0600
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
)

target_include_directories(
    PicoTorrent.Core
    PRIVATE
    include/
)

target_link_libraries(
    PicoTorrent.Core
    PRIVATE
    shlwapi
    
    # sqlite
    sqlite
)

add_executable(
    PicoTorrent
    WIN32

    src/picotorrent/aboutdialog
    src/picotorrent/addtorrentdialog
    src/picotorrent/application
    src/picotorrent/connectionsectionwidget
    src/picotorrent/downloadssectionwidget
    src/picotorrent/elidedlabel
    src/picotorrent/filestorageitemdelegate
    src/picotorrent/filestorageitemmodel
    src/picotorrent/generalsectionwidget
    src/picotorrent/main
    src/picotorrent/mainwindow
    src/picotorrent/peerlistmodel
    src/picotorrent/preferencesdialog
    src/picotorrent/proxysectionwidget
    src/picotorrent/resources.qrc
    src/picotorrent/resources.rc
    src/picotorrent/sessionloader
    src/picotorrent/sessionunloader
    src/picotorrent/statusbar
    src/picotorrent/systemtrayicon
    src/picotorrent/torrentcontextmenu
    src/picotorrent/torrentitemdelegate
    src/picotorrent/torrentlistmodel
    src/picotorrent/torrentlistwidget
    src/picotorrent/translator

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
    -DPICO_API_EXPORT
    -DPICO_GIT_COMMITISH=${GIT_COMMITISH}
    -DPICO_VERSION_MAJOR=${VERSION_MAJOR}
    -DPICO_VERSION_MINOR=${VERSION_MINOR}
    -DPICO_VERSION_REVISION=${VERSION_REVISION}
)

target_include_directories(
    PicoTorrent
    PRIVATE
    include/
    "${BOOST_INCLUDE_DIRS}"
    "${CMAKE_SOURCE_DIR}/vendor/libtorrent/include"
)

target_link_libraries(
    PicoTorrent

    # Own libraries
    PicoTorrent.Core
    PicoTorrent.GeoIP
    PicoTorrent.Http

    # Windows
    crypt32
    iphlpapi
    legacy_stdio_definitions
    shlwapi

    # Boost
    boost-system

    # Breakpad
    breakpad

    # Rasterbar-libtorrent
    libtorrent-rasterbar

    # OpenSSL
    OpenSSL::Crypto
    OpenSSL::SSL

    # Qt
    Qt5::Svg
    Qt5::Widgets
)

# Copy base Qt dependencies
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Core>
        $<TARGET_FILE:Qt5::Gui>
        $<TARGET_FILE:Qt5::Svg>
        $<TARGET_FILE:Qt5::Widgets>
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
