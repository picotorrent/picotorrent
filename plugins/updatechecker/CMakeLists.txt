
add_library(
    UpdateChecker
    SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/Controllers/CheckForUpdateController
    ${CMAKE_CURRENT_LIST_DIR}/src/Net/HttpClient
    ${CMAKE_CURRENT_LIST_DIR}/src/UpdateChecker
)

target_link_libraries(
    UpdateChecker
    WinHTTP
)
