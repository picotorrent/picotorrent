#pragma once

#include <windows.h>

// Messages
#define PT_ALERT WM_USER+1
#define PT_PAUSETORRENTS WM_USER+2
#define PT_RESUMETORRENTS WM_USER+3
#define PT_MOVETORRENTS WM_USER+4
#define PT_REMOVETORRENTS WM_USER+5
#define PT_SHOWTORRENTDETAILS WM_USER+6
#define PT_PRIORITIZEFILES WM_USER+7
#define PT_ADDTRACKER WM_USER+8
#define PT_REMOVETRACKERS WM_USER+9
#define PT_QUEUETORRENT WM_USER+10
#define PT_FINDMETADATA WM_USER+11
#define PT_METADATAFOUND WM_USER+12
#define PT_TORRENT_UPDATED WM_USER+100
#define PT_NOTIFYICON WM_USER+1000

#define PT_REGISTERNOTIFY WM_USER+1001
#define PT_UNREGISTERNOTIFY WM_USER+1002


#define TEXTFILE 256

#define PRIORITY_DO_NOT_DOWNLOAD 0
#define PRIORITY_NORMAL 4
#define PRIORITY_HIGH 6
#define PRIORITY_MAXIMUM 7

// Keyboard accelerators
#define IDR_PICO_ACCELERATORS 8000
#define IDA_SELECT_ALL 8001
#define IDA_REMOVE_TORRENTS 8002
#define IDA_REMOVE_TORRENTS_DATA 8003

// Application
#define IDR_MAINMENU 101
#define IDI_APPICON  201

// Dialogs
#define IDD_PREFERENCES 301
#define IDD_ABOUT 302
#define IDD_ADD_TORRENT 303
#define IDD_ADD_MAGNET_LINK 304
#define IDD_ADD_TRACKER 305
#define IDD_DETAILS_OVERVIEW 310
#define IDD_DETAILS_FILES 311
#define IDD_DETAILS_PEERS 312
#define IDD_DETAILS_TRACKERS 313
#define IDD_PREFERENCES_DOWNLOADS 314
#define IDD_PREFERENCES_CONNECTION 315
#define IDD_PREFERENCES_ADVANCED 316
#define IDD_PREFERENCES_GENERAL 317
#define IDD_PREFERENCES_REMOTE 318
#define IDD_PREFERENCES_PLUGINS 319

#define IDD_DETAILS_OPTIONS 330
#define IDD_REMOTE_QR 340

// Menus
#define ID_FILE_ADD_TORRENT 9001
#define ID_FILE_ADD_MAGNET_LINK 9002
#define ID_FILE_EXIT 9003
#define ID_VIEW_PREFERENCES 9101
#define ID_HELP_ABOUT 9201
#define ID_HELP_CHECK_FOR_UPDATE 9203

// Add torrent dialog
#define ID_TORRENT_TEXT 101
#define ID_TORRENT 102
#define ID_SIZE_TEXT 103
#define ID_SIZE 104
#define ID_STORAGE_GROUP 105
#define ID_SAVE_PATH_TEXT 106
#define ID_SAVE_PATH 107
#define ID_BROWSE 108
#define ID_FILES 109
#define ID_ADD_STORAGE_MODE_TEXT 110
#define ID_ADD_STORAGE_MODE_SPARSE 111
#define ID_ADD_STORAGE_MODE_FULL 112

// Add magnet link dialog
#define ID_MAGNET_LINKS_GROUP 101
#define ID_MAGNET_LINKS_TEXT 102
#define ID_MAGNET_PROGRESS 103
#define ID_MAGNET_CURRENT_STATUS 104
#define ID_MAGNET_ADD_LINKS 105

// Add tracker dialog
#define ID_ADD_TRACKER_URL 101

// General preferences dialog
#define ID_UI_GROUP 101
#define ID_LANGUAGE_TEXT 102
#define ID_LANGUAGE 103
#define ID_MISC_GROUP 104
#define ID_AUTOSTART_PICO 105
#define ID_START_POSITION_TEXT 106
#define ID_START_POSITION 107

// Download preferences dialog
#define ID_TRANSFERS_GROUP 102
#define ID_DEFSAVEPATH_TEXT 103
#define ID_PREFS_DEFSAVEPATH 9101
#define ID_PREFS_DEFSAVEPATH_BROWSE 9104
#define ID_DL_PREFS_LIMITS_GROUP 104
#define ID_PREFS_GLOBAL_DL_LIMIT_TEXT 105
#define ID_PREFS_GLOBAL_DL_LIMIT 106
#define ID_PREFS_GLOBAL_DL_LIMIT_HELP 107
#define ID_PREFS_GLOBAL_UL_LIMIT_TEXT 108
#define ID_PREFS_GLOBAL_UL_LIMIT 109
#define ID_PREFS_GLOBAL_UL_LIMIT_HELP 110

// Connection preferences dialog
#define ID_LISTEN_INTERFACE_GROUP 101
#define ID_LISTEN_INTERFACES 105
#define ID_PROXY_GROUP 106
#define ID_TYPE_TEXT 107
#define ID_PROXY_TYPE 108
#define ID_HOST_TEXT 109
#define ID_PROXY_HOST 110
#define ID_PROXY_PORT_TEXT 111
#define ID_PROXY_PORT 112
#define ID_PROXY_USERNAME_TEXT 113
#define ID_PROXY_USERNAME 114
#define ID_PROXY_PASSWORD_TEXT 115
#define ID_PROXY_PASSWORD 116
#define ID_PROXY_FORCE 117
#define ID_PROXY_HOSTNAMES 118
#define ID_PROXY_PEERS 119
#define ID_PROXY_TRACKERS 120

// Remote preferences dialog
#define ID_REMOTE_OPTIONS_GROUP 101
#define ID_REMOTE_ENABLE 102
#define ID_REMOTE_PORT_TEXT 103
#define ID_REMOTE_PORT 104
#define ID_REMOTE_SECURITY_GROUP 105
#define ID_REMOTE_TOKEN_TEXT 106
#define ID_REMOTE_TOKEN 107
#define ID_REMOTE_CERT_TEXT 108
#define ID_REMOTE_CERT_PUBKEY 109
#define ID_REMOTE_SHOW_QR 110

// Plugins preferences dialog
#define ID_PLUGINS_COMBO 101

// Advanced preferences dialog
#define ID_EXPERIMENTAL_GROUP 101
#define ID_USE_PICO_ID 102

// Overview details dialog
#define ID_OVERVIEW_STATISTICS_GROUP 101
#define ID_OVERVIEW_RATIO_TEXT 102
#define ID_OVERVIEW_RATIO 103
#define ID_OVERVIEW_PIECES_TEXT 104
#define ID_OVERVIEW_PIECES 105
#define ID_OVERVIEW_DOWNLOADED_TEXT 106
#define ID_OVERVIEW_DOWNLOADED 107
#define ID_OVERVIEW_UPLOADED_TEXT 108
#define ID_OVERVIEW_UPLOADED 109

// Option details dialog
#define ID_LIMITS_GROUP 101
#define ID_DL_LIMIT_TEXT 102
#define ID_DL_LIMIT 103
#define ID_DL_LIMIT_HELP 104
#define ID_UL_LIMIT_TEXT 105
#define ID_UL_LIMIT 106
#define ID_UL_LIMIT_HELP 107
#define ID_MAX_CONNECTIONS_TEXT 108
#define ID_MAX_CONNECTIONS 109
#define ID_MAX_CONNECTIONS_HELP 110
#define ID_MAX_UPLOADS_TEXT 111
#define ID_MAX_UPLOADS 112
#define ID_MAX_UPLOADS_HELP 113
#define ID_SEQUENTIAL_DOWNLOAD 114

#define ID_DETAILS_FILES_LIST 1100
#define ID_DETAILS_PEERS_LIST 1101
#define ID_DETAILS_TRACKERS_LIST 1102

// Torrent context menu
#define IDR_TORRENT_CONTEXT_MENU 4100
#define TORRENT_CONTEXT_MENU_PAUSE 4101
#define TORRENT_CONTEXT_MENU_RESUME 4102
#define TORRENT_CONTEXT_MENU_RESUME_FORCE 4103
#define TORRENT_CONTEXT_MENU_MOVE 4104
#define TORRENT_CONTEXT_MENU_REMOVE 4105
#define TORRENT_CONTEXT_MENU_REMOVE_DATA 4106
#define TORRENT_CONTEXT_MENU_COPY_SHA 4107
#define TORRENT_CONTEXT_MENU_COPY_MAGNET 4108
#define TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER 4109
#define TORRENT_CONTEXT_MENU_DETAILS 4110
#define TORRENT_CONTEXT_MENU_QUEUE_UP 4111
#define TORRENT_CONTEXT_MENU_QUEUE_DOWN 4112
#define TORRENT_CONTEXT_MENU_QUEUE_TOP 4113
#define TORRENT_CONTEXT_MENU_QUEUE_BOTTOM 4114

// Torrent file context menu
#define IDR_TORRENT_FILE_MENU 4400
#define TORRENT_FILE_PRIO_SKIP 4401
#define TORRENT_FILE_PRIO_NORMAL 4402
#define TORRENT_FILE_PRIO_HIGH 4403
#define TORRENT_FILE_PRIO_MAX 4404

// NotifyIcon context menu
#define IDR_NOTIFYICON_CONTEXT_MENU 4200
#define ID_NOTIFYICON_ADD 4201
#define ID_NOTIFYICON_ADD_MAGNET 4202
#define ID_NOTIFYICON_EXIT 4203

// About dialog
#define ID_PICO_LOGO 101
#define ID_PICOTORRENT_V_FORMAT 102
#define ID_BUILD_INFO_FORMAT 103
#define ID_PICOTORRENT_DESCRIPTION 104
#define ID_GITHUB_LINK 105

// Remote QR dialog
#define ID_QR_CONTROL 101

// Keyboard shortcuts
#define IDHOT_SELECT_ALL 1001
