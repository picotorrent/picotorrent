
add_library(
    JavaScript
    SHARED

    ${CMAKE_CURRENT_LIST_DIR}/src/duk/Context
    ${CMAKE_CURRENT_LIST_DIR}/src/duk/duktape

    ${CMAKE_CURRENT_LIST_DIR}/src/Engine/JavaScriptEngine
    ${CMAKE_CURRENT_LIST_DIR}/src/JavaScript
)

target_link_libraries(
    JavScript
)
