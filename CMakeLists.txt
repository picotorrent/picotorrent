project("PicoTorrent")
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Set dependency directories
set(BOOST_ROOT      $ENV{BOOST_ROOT})
set(LIBTORRENT_ROOT $ENV{LIBTORRENT_ROOT})

set(includes
    include/wtl-9.0
    ${BOOST_ROOT}
    ${LIBTORRENT_ROOT}/include
)

set(PICOTORRENT_SOURCES
    src/commandline
    src/env
    src/logging
    src/main
    src/path
    src/picotorrent
    src/resources.rc
    src/statemanager
    src/util
    src/controllers/addtorrentcontroller
    src/io/file
    src/ui/addtorrentdialog
    src/ui/mainframe
    src/ui/renamefiledialog
    src/ui/scaler
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4714 /W4 /WX")

include_directories(${includes})

link_directories(
    ${BOOST_ROOT}/stage/lib
    ${LIBTORRENT_ROOT}/bin/msvc-14.0/$(ConfigurationName)/address-model-64/deprecated-functions-off/link-static/threading-multi
)

add_definitions(
    -DBOOST_ASIO_SEPARATE_COMPILATION
    -DUNICODE
    -D_UNICODE
    -DTORRENT_NO_DEPRECATE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
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

