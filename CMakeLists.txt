project("PicoTorrent")
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Set dependency directories
set(WTL_ROOT        $ENV{WTL_ROOT})
set(BOOST_ROOT      $ENV{BOOST_ROOT})
set(LIBTORRENT_ROOT $ENV{LIBTORRENT_ROOT})

set(includes
    ${WTL_ROOT}
    ${BOOST_ROOT}
    ${LIBTORRENT_ROOT}/include
)

set(PICOTORRENT_SOURCES
    src/main
    src/picotorrent
    src/resources.rc
    src/ui/mainwindow
    src/ui/torrentdetailsframe
    src/ui/torrentlistviewctrl
)

include_directories(${includes})

link_directories(
    ${BOOST_ROOT}/stage/lib
    ${LIBTORRENT_ROOT}/bin/msvc-12.0/$(ConfigurationName)/boost-source/link-static/threading-multi
)

add_definitions(
    -DBOOST_ASIO_SEPARATE_COMPILATION
    -DUNICODE
    -D_UNICODE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
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
)

