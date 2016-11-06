
add_library(
    WebSocket
    SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/Messages/PicoStateMessage
    ${CMAKE_CURRENT_LIST_DIR}/src/Serialization/TorrentSerializer
    ${CMAKE_CURRENT_LIST_DIR}/src/TorrentEventSink
    ${CMAKE_CURRENT_LIST_DIR}/src/WebSocket
)

target_include_directories(
    WebSocket PRIVATE lib/websocketpp
)

target_link_libraries(
    WebSocket

    debug boost_system-vc140-mt-gd-1_61
    optimized boost_system-vc140-mt-1_61
)
