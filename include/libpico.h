#ifndef LIBPICO_H
#define LIBPICO_H

#include <stdint.h>

#define LIBPICO_API_VERSION 1
#define LIBPICO_INIT libpico_init
#define LIBPICO_STRINGIFY(x) #x
#define LIBPICO_TOSTRING(x) LIBPICO_STRINGIFY(x)

#define LIBPICO_DEFINE_PLUGIN(name,version,init_func) \
    extern "C" __declspec(dllexport) libpico_result_t LIBPICO_INIT(int v, libpico_plugin_t* a) \
    { \
        return init_func(v,a); \
    }

#if defined(LIBPICO_API_EXPORT)
#define LIBPICO_API_FUNCTION \
    extern "C" __declspec(dllexport)
#else
#define LIBPICO_API_FUNCTION \
    extern "C" __declspec(dllimport)
#endif

typedef struct libpico_config_t libpico_config_t;
typedef struct libpico_http_response_t libpico_http_response_t;
typedef struct libpico_mainwnd_t libpico_mainwnd_t;
typedef struct libpico_menu_t libpico_menu_t;
typedef struct libpico_menuitem_t libpico_menuitem_t;
typedef struct libpico_param_t libpico_param_t;
typedef struct libpico_plugin_t libpico_plugin_t;
typedef struct libpico_torrent_t libpico_torrent_t;

enum libpico_event_t
{
    libpico_event_mainwnd_created,
    libpico_event_shutdown
};

enum libpico_http_status_t
{
    libpico_http_ok = 200,
    libpico_http_not_found = 404,
    libpico_http_internal_server_error = 500
};

enum libpico_menu_id_t
{
    libpico_menu_file,
    libpico_menu_view,
    libpico_menu_help
};

enum libpico_result_t
{
    libpico_ok,
    libpico_err,
    libpico_version_mismatch,
    libpico_insufficient_buffer
};

typedef struct libpico_torrent_stats_t
{
    int32_t download_payload_rate;
    int32_t upload_payload_rate;
} libpico_torrent_stats_t;

typedef bool(*libpico_filter_callback_t)(libpico_torrent_t*, libpico_param_t*);
typedef libpico_result_t(*libpico_http_callback_t)(libpico_http_response_t*, libpico_http_status_t, libpico_param_t*);
typedef libpico_result_t(*libpico_init_t)(int, libpico_plugin_t*);
typedef libpico_result_t(*libpico_hook_callback_t)(libpico_event_t, libpico_param_t*, libpico_param_t*);
typedef libpico_result_t(*libpico_menuitem_callback_t)(libpico_menuitem_t*, libpico_param_t*);

/*
Config.
*/
LIBPICO_API_FUNCTION libpico_result_t libpico_config_get(libpico_plugin_t* plugin, libpico_config_t** config);
LIBPICO_API_FUNCTION libpico_result_t libpico_config_bool_get(libpico_config_t* config, const char* key, bool* result);
LIBPICO_API_FUNCTION libpico_result_t libpico_config_string_get(libpico_config_t* config, const char* key, char* result, size_t* len);
LIBPICO_API_FUNCTION libpico_result_t libpico_config_string_set(libpico_config_t* config, const char* key, const char* value, size_t len);

/*
HTTP.
*/
LIBPICO_API_FUNCTION libpico_result_t libpico_http_get(const char* url, libpico_http_callback_t cb, libpico_param_t * user);
LIBPICO_API_FUNCTION libpico_result_t libpico_http_response_body(libpico_http_response_t* response, char* body, size_t len);
LIBPICO_API_FUNCTION libpico_result_t libpico_http_response_body_len(libpico_http_response_t* response, size_t* len);

LIBPICO_API_FUNCTION libpico_result_t libpico_i18n(const char* key, wchar_t* target, size_t* len);

LIBPICO_API_FUNCTION libpico_result_t libpico_mainwnd_filter_add(libpico_mainwnd_t* wnd, libpico_filter_callback_t cb, const wchar_t* name, libpico_param_t* user);
LIBPICO_API_FUNCTION libpico_result_t libpico_mainwnd_native_handle(libpico_mainwnd_t* wnd, void** handle);

/*
Menu.
*/
LIBPICO_API_FUNCTION libpico_result_t libpico_menu_get(libpico_mainwnd_t* wnd, libpico_menu_id_t id, libpico_menu_t** menu);
LIBPICO_API_FUNCTION libpico_result_t libpico_menu_insert_item(libpico_menu_t* menu, uint32_t pos, const wchar_t* label, size_t len, libpico_menuitem_callback_t cb, libpico_param_t* param, libpico_menuitem_t** item);
LIBPICO_API_FUNCTION libpico_result_t libpico_menu_insert_separator(libpico_menu_t* menu, uint32_t pos);

/*
String functions
*/
LIBPICO_API_FUNCTION libpico_result_t libpico_string_towide(const char* input, wchar_t* output, size_t len);

LIBPICO_API_FUNCTION libpico_result_t libpico_register_hook(libpico_plugin_t* app, libpico_hook_callback_t cb, libpico_param_t* user);
LIBPICO_API_FUNCTION libpico_result_t libpico_torrent_stats_get(libpico_torrent_t* torrent, libpico_torrent_stats_t* stats);

LIBPICO_API_FUNCTION const char* libpico_version();
#endif
