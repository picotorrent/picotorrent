cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("PicoTorrent")

set(PICOTORRENT_SOURCES
    src/application
    src/main
    src/mainwindow
    src/scaler
    src/torrentlistview
)

add_executable(
    PicoTorrent
    WIN32
    ${PICOTORRENT_SOURCES}
)

target_link_libraries(
    PicoTorrent
    comctl32
)
