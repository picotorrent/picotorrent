
add_library(
    Importer
    SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/Controllers/ImportTorrentsController
    ${CMAKE_CURRENT_LIST_DIR}/src/Importer
    ${CMAKE_CURRENT_LIST_DIR}/src/resources.rc
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/ImportWizard
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/PreviewPage
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/QBittorrentPage
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/UTorrentPage
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/WelcomePage
)

target_link_libraries(
    Importer
    Comctl32
)
