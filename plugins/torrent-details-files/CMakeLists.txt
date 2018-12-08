add_library(
    torrent-details-files
    SHARED
    src/plugin
)

add_custom_command(
    TARGET torrent-details-files POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        $<TARGET_FILE_DIR:torrent-details-files>
        "$<TARGET_FILE_DIR:PicoTorrent>/plugins/torrent-details-files"
)

target_include_directories(
    torrent-details-files
    PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(
    torrent-details-files
    Qt5::Widgets
)
