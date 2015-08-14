project("PicoTorrent")
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Set dependency directories
set(BOOST_ROOT      $ENV{BOOST_ROOT})
set(LIBTORRENT_ROOT $ENV{LIBTORRENT_ROOT})
set(RAPIDJSON_ROOT  $ENV{RAPIDJSON_ROOT})
set(WXWIDGETS_ROOT  $ENV{WXWIN})

set(includes
    ${BOOST_ROOT}
    ${LIBTORRENT_ROOT}/include
    ${RAPIDJSON_ROOT}/include
    ${WXWIDGETS_ROOT}/include
    ${WXWIDGETS_ROOT}/include/msvc
)

set(PICOTORRENT_SOURCES
    src/main
    src/common
    src/config
    src/fsutil
    src/picotorrent
    src/ui/addtorrentdialog
    src/ui/mainframe
    src/ui/torrentlistctrl
    src/ui/torrentdetailsframe
)

if(WIN32)
    list(APPEND
         PICOTORRENT_SOURCES
         src/_win32/platform
         src/_win32/resource.rc)
endif(WIN32)

include_directories(${includes})

link_directories(
    ${BOOST_ROOT}/stage/lib
    ${LIBTORRENT_ROOT}/bin/msvc-14.0/$(ConfigurationName)/boost-source/deprecated-functions-off/link-static/threading-multi
    ${WXWIDGETS_ROOT}/lib/vc_lib
)

add_definitions(
    -DBOOST_ASIO_SEPARATE_COMPILATION
    -DUNICODE
    -D_UNICODE
    -DTORRENT_NO_DEPRECATE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
    -D_WIN32_WINNT=0x0600
    -D_CRT_SECURE_NO_WARNINGS
    -DRAPIDJSON_HAS_STDSTRING
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

