cmake_minimum_required(VERSION 3.20...4.2 FATAL_ERROR)

# set(VCPKG_OVERLAY_PORTS    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg-overlays/ports)
set(VCPKG_OVERLAY_TRIPLETS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg-overlays/triplets)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

project("PicoTorrent")

set(CMAKE_CXX_STANDARD 17)

# --------- antlr4 options
option(WITH_STATIC_CRT "" Off)
# -----------------------

# --------- wx options
set(wxBUILD_SHARED        OFF   CACHE BOOL   "" FORCE)
set(wxBUILD_COMPATIBILITY "3.2" CACHE STRING "" FORCE)
set(wxUSE_EXCEPTIONS      OFF   CACHE BOOL   "" FORCE)
# --------------------

# vendor libs
add_subdirectory(vendor/wx)

# vcpkg packages
find_package(Boost                      REQUIRED COMPONENTS log)
find_package(fmt                 CONFIG REQUIRED)
find_package(LibtorrentRasterbar CONFIG REQUIRED)
find_package(nlohmann_json       CONFIG REQUIRED)
find_package(unofficial-sqlite3  CONFIG REQUIRED)
find_package(utf8cpp            CONFIG REQUIRED)

# pico components
add_subdirectory(src/pql)

# Generate buildinfo file
configure_file("${CMAKE_SOURCE_DIR}/src/picotorrent/buildinfo.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/buildinfo.cpp" @ONLY)

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/DEBUG /INCREMENTAL:NO /MAP /OPT:REF /OPT:ICF")

add_executable(
    PicoTorrent-coredb
    src/l10n/main.cpp
    lang/ca-ES.json
    lang/de-DE.json
    lang/el-GR.json
    lang/en-US.json
    lang/es-ES.json
    lang/fi-FI.json
    lang/fr-FR.json
    lang/hu-HU.json
    lang/hy-AM.json
    lang/id-ID.json
    lang/it-IT.json
    lang/ka-GE.json
    lang/ko-KR.json
    lang/lt-LT.json
    lang/lv-LV.json
    lang/nl-NL.json
    lang/pl-PL.json
    lang/pt-BR.json
    lang/ro-RO.json
    lang/ru-RU.json
    lang/sv-SE.json
    lang/tr-TR.json
    lang/uk-UA.json
    lang/zh-CN.json
)

target_link_libraries(
    PicoTorrent-coredb
    PRIVATE
    nlohmann_json::nlohmann_json
    unofficial::sqlite3::sqlite3
)

add_executable(
    PicoTorrent
    WIN32

    src/picotorrent/application.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/buildinfo.cpp"
    src/picotorrent/main.cpp
    src/picotorrent/persistencemanager.cpp
    src/picotorrent/resources.rc

    # API
    src/picotorrent/api/libpico.cpp
    # BitTorrent
    src/picotorrent/bittorrent/session.cpp
    src/picotorrent/bittorrent/torrenthandle.cpp
    # Core
    src/picotorrent/core/configuration.cpp
    src/picotorrent/core/database.cpp
    src/picotorrent/core/environment.cpp
    src/picotorrent/core/utils.cpp
    # Http
    src/picotorrent/http/httpclient.cpp
    # IPC
    src/picotorrent/ipc/applicationoptionsconnection.cpp
    src/picotorrent/ipc/server.cpp
    # Dialogs
    src/picotorrent/ui/dialogs/aboutdialog.cpp
    src/picotorrent/ui/dialogs/addmagnetlinkdialog.cpp
    src/picotorrent/ui/dialogs/addtrackerdialog.cpp
    src/picotorrent/ui/dialogs/addtorrentdialog.cpp
    src/picotorrent/ui/dialogs/createtorrentdialog.cpp
    src/picotorrent/ui/dialogs/listeninterfacedialog.cpp
    src/picotorrent/ui/dialogs/preferencesadvancedpage.cpp
    src/picotorrent/ui/dialogs/preferencesconnectionpage.cpp
    src/picotorrent/ui/dialogs/preferencesdialog.cpp
    src/picotorrent/ui/dialogs/preferencesdownloadspage.cpp
    src/picotorrent/ui/dialogs/preferencesgeneralpage.cpp
    src/picotorrent/ui/dialogs/preferenceslabelspage.cpp
    src/picotorrent/ui/dialogs/preferencesproxypage.cpp
    src/picotorrent/ui/dialogs/textoutputdialog.cpp
    # Filters
    src/picotorrent/ui/filters/pqltorrentfilter.cpp
    # Models
    src/picotorrent/ui/models/filestoragemodel.cpp
    src/picotorrent/ui/models/peerlistmodel.cpp
    src/picotorrent/ui/models/torrentlistmodel.cpp
    src/picotorrent/ui/models/trackerlistmodel.cpp
    # Persistence
    src/picotorrent/ui/persistence/persistenttorrentlistview.cpp
    # UI
    src/picotorrent/ui/console.cpp
    src/picotorrent/ui/mainframe.cpp
    src/picotorrent/ui/statusbar.cpp
    src/picotorrent/ui/taskbaricon.cpp
    src/picotorrent/ui/torrentcontextmenu.cpp
    src/picotorrent/ui/torrentdetailsfilespanel.cpp
    src/picotorrent/ui/torrentdetailsoverviewpanel.cpp
    src/picotorrent/ui/torrentdetailspeerspanel.cpp
    src/picotorrent/ui/torrentdetailstrackerspanel.cpp
    src/picotorrent/ui/torrentdetailsview.cpp
    src/picotorrent/ui/torrentfilelistview.cpp
    src/picotorrent/ui/torrentlistview.cpp
    src/picotorrent/ui/translator.cpp
    # Widgets
    src/picotorrent/ui/widgets/pieceprogressbar.cpp
    # Win32 specific stuff
    src/picotorrent/ui/win32/openfiledialog.cpp
)

target_compile_definitions(
    PicoTorrent
    PRIVATE
    -D_UNICODE
    -D_WIN32
    -D_WIN32_WINNT=0x0A00
    -DNOMINMAX
    -DPICOJSON_USE_INT64
    -DUNICODE
    -DWIN32
    -DWIN32_LEAN_AND_MEAN

    -DLIBPICO_API_EXPORT

    -DPICO_GIT_COMMITISH=${GITVERSION_VAR_SHORTSHA}
    -DPICO_VERSION_MAJOR=${GITVERSION_VAR_VERSION_MAJOR}
    -DPICO_VERSION_MINOR=${GITVERSION_VAR_VERSION_MINOR}
    -DPICO_VERSION_PATCH=${GITVERSION_VAR_VERSION_PATCH}

    -DPICO_WX_GIT_COMMITISH=${PICO_WX_GIT_COMMITISH}
)

target_compile_options(PicoTorrent PRIVATE /guard:cf /W4 /WX)

target_include_directories(
    PicoTorrent
    PRIVATE

    include
    src/picotorrent
)

target_link_libraries(
    PicoTorrent

    PRIVATE

    # boost
    Boost::boost
    Boost::log

    # wxWidgets
    wxcore wxbase wxpropgrid

    # Windows
    Comctl32
    crypt32
    iphlpapi
    legacy_stdio_definitions
    propsys
    shlwapi
    wininet
    winhttp

    # fmt
    fmt::fmt

    # nlohmann-json
    nlohmann_json::nlohmann_json

    # Rasterbar-libtorrent
    LibtorrentRasterbar::torrent-rasterbar

    # PQL
    PicoTorrentPQL

    unofficial::sqlite3::sqlite3
)

set_property(TARGET PicoTorrent PROPERTY ENABLE_EXPORTS 1)

# Plugins
add_library(
    Plugin_Updater
    SHARED
    src/plugins/updater/updater.cpp
)

target_compile_definitions(Plugin_Updater PRIVATE -D_WIN32 -DUNICODE -DWIN32_LEAN_AND_MEAN)
target_include_directories(Plugin_Updater PRIVATE include)
target_link_libraries(Plugin_Updater PRIVATE PicoTorrent Comctl32)

# Generate coredb
add_custom_command(
    TARGET PicoTorrent POST_BUILD
    COMMAND PicoTorrent-coredb "${CMAKE_SOURCE_DIR}/lang" $<TARGET_FILE_DIR:PicoTorrent>
)
