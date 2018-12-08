add_library(
    torrent-details-peers
    SHARED
    src/plugin
)

add_custom_command(
    TARGET torrent-details-peers POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        $<TARGET_FILE_DIR:torrent-details-peers>
        "$<TARGET_FILE_DIR:PicoTorrent>/plugins/torrent-details-peers"
)

target_include_directories(
    torrent-details-peers
    PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(
    torrent-details-peers
    Qt5::Widgets
)
