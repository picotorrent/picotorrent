
add_library(
    Importer
    SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/Controllers/ImportTorrentsController
    ${CMAKE_CURRENT_LIST_DIR}/src/Importer
    ${CMAKE_CURRENT_LIST_DIR}/src/resources.rc
    ${CMAKE_CURRENT_LIST_DIR}/src/Sources/qBittorrentSource
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/ConfigurePage
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/ImportWizard
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/PreviewPage
    ${CMAKE_CURRENT_LIST_DIR}/src/Wizard/WelcomePage
)

target_link_libraries(
    Importer
    Comctl32

    debug boost_system-vc140-mt-gd-1_61
    optimized boost_system-vc140-mt-1_61
    torrent
)
