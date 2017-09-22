cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("PicoTorrent")

add_definitions(
    -D_UNICODE
    -D_WIN32
    -D_WIN32_WINNT=0x0600
    -DUNICODE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN
)

include_directories(
    C:/Code/wxWidgets/include
    C:/Code/wxWidgets/include/msvc
)

link_directories(
    C:/Code/wxWidgets/lib/vc_x64_lib
)

add_executable(
    PicoTorrent
    WIN32
    src/picotorrent/application
    src/picotorrent/main
    src/picotorrent/mainframe
    src/picotorrent/torrentdetailsview
    src/picotorrent/torrentlistview
)
