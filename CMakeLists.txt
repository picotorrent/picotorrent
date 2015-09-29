cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("PicoTorrent")

set(BOOST_ROOT      $ENV{BOOST_ROOT})
set(LIBTORRENT_ROOT $ENV{LIBTORRENT_ROOT})
set(OPENSSL_ROOT    $ENV{OPENSSL_ROOT})

set(PICOTORRENT_SOURCES
    src/application
    src/main
    src/mainwindow
    src/resource.rc
    src/scaler
    src/torrentitem
    src/torrentlistview
)

include_directories(
    ${BOOST_ROOT}
    ${LIBTORRENT_ROOT}/include
    ${OPENSSL_ROOT}/inc32
)

link_directories(
    ${BOOST_ROOT}/stage/lib
    ${LIBTORRENT_ROOT}/bin/msvc-14.0/$(ConfigurationName)/address-model-64/crypto-openssl/deprecated-functions-off/link-static/threading-multi
    ${OPENSSL_ROOT}/out32
)

add_definitions(
    -D_UNICODE
    -D_WIN32
    -DBOOST_ALL_NO_LIB
    -DTORRENT_NO_DEPRECATE
    -DTORRENT_USE_OPENSSL
    -DUNICODE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
)

add_executable(
    PicoTorrent
    WIN32
    ${PICOTORRENT_SOURCES}
)

target_link_libraries(
    PicoTorrent
    comctl32
    iphlpapi
    shlwapi
    uxtheme

    debug dbghelp

    # Boost.Random
    debug     libboost_random-vc140-mt-gd-1_59
    optimized libboost_random-vc140-mt-1_59

    # Boost.System
    debug     libboost_system-vc140-mt-gd-1_59
    optimized libboost_system-vc140-mt-1_59

    # OpenSSL
    libeay32
    ssleay32

    # Rasterbar-libtorrent
    libtorrent
)
