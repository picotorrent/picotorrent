project("PicoTorrent")
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Set dependency directories
set(BOOST_ROOT      $ENV{BOOST_ROOT})
set(LIBTORRENT_ROOT $ENV{LIBTORRENT_ROOT})
set(WXWIDGETS_ROOT  $ENV{WXWIN})

set(includes
    ${BOOST_ROOT}
    ${LIBTORRENT_ROOT}/include
    ${WXWIDGETS_ROOT}/include
    ${WXWIDGETS_ROOT}/include/msvc
    C:/Python34/include
)

set(PICOTORRENT_SOURCES
    src/main
    src/common
    src/picotorrent
    src/scripting/pyhost
    src/ui/addtorrentdialog
    src/ui/mainframe
    src/ui/logframe
    src/ui/torrentdetailsframe

    # libtorrent bindings
    src/scripting/bindings/module.cpp
    src/scripting/bindings/big_number.cpp
    src/scripting/bindings/converters.cpp
    src/scripting/bindings/create_torrent.cpp
    src/scripting/bindings/fingerprint.cpp
    src/scripting/bindings/utility.cpp
    src/scripting/bindings/session.cpp
    src/scripting/bindings/entry.cpp
    src/scripting/bindings/torrent_info.cpp
    src/scripting/bindings/string.cpp
    src/scripting/bindings/torrent_handle.cpp
    src/scripting/bindings/torrent_status.cpp
    src/scripting/bindings/session_settings.cpp
    src/scripting/bindings/version.cpp
    src/scripting/bindings/alert.cpp
    src/scripting/bindings/datetime.cpp
    src/scripting/bindings/peer_info.cpp
    src/scripting/bindings/ip_filter.cpp
    src/scripting/bindings/magnet_uri.cpp
    src/scripting/bindings/error_code.cpp
)

if(WIN32)
    list(APPEND
         PICOTORRENT_SOURCES
         src/_win32/resource.rc)
endif(WIN32)

include_directories(${includes})

link_directories(
    ${BOOST_ROOT}/stage/lib
    ${LIBTORRENT_ROOT}/bin/msvc-14.0/$(ConfigurationName)/boost-source/deprecated-functions-off/link-static/threading-multi
    ${WXWIDGETS_ROOT}/lib/vc_lib
    C:/Python34/libs
)

add_definitions(
    -DBOOST_ASIO_SEPARATE_COMPILATION
    -DBOOST_PYTHON_STATIC_LIB
    -DUNICODE
    -D_UNICODE
    -DTORRENT_NO_DEPRECATE
    -DWITH_SHIPPED_GEOIP_H
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
    -D_WIN32_WINNT=0x0600
    -D_CRT_SECURE_NO_WARNINGS
    /MP
)

add_executable(
    PicoTorrent
    WIN32
    ${PICOTORRENT_SOURCES}
)

target_link_libraries(
    PicoTorrent
    dbghelp
    libtorrent
    iphlpapi
)

