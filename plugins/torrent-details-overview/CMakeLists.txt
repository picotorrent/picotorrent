add_library(
    torrent-details-overview
    SHARED
    src/plugin
)

add_custom_command(
    TARGET torrent-details-overview POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        $<TARGET_FILE_DIR:torrent-details-overview>
        "$<TARGET_FILE_DIR:PicoTorrent>/plugins/torrent-details-overview"
)

target_include_directories(
    torrent-details-overview
    PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(
    torrent-details-overview
    Qt5::Widgets
)
