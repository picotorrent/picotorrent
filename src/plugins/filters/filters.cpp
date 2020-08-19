#include <libpico.h>

#define DEFAULT_BUFFER_SIZE 100

bool filter_downloading(libpico_torrent_t* torrent, libpico_param_t* user)
{
    libpico_torrent_stats_t stats;
    libpico_torrent_stats_get(torrent, &stats);
    return stats.download_payload_rate > 0;
}

bool filter_uploading(libpico_torrent_t* torrent, libpico_param_t* user)
{
    libpico_torrent_stats_t stats;
    libpico_torrent_stats_get(torrent, &stats);
    return stats.upload_payload_rate > 0;
}

libpico_result_t on_events(
    libpico_event_t event,
    libpico_param_t* param,
    libpico_param_t* user)
{
    switch (event)
    {
    case libpico_event_mainwnd_created:
    {
        libpico_mainwnd_t* mainwnd = reinterpret_cast<libpico_mainwnd_t*>(param);

        wchar_t downloading[DEFAULT_BUFFER_SIZE];
        size_t downloading_len = DEFAULT_BUFFER_SIZE;
        wchar_t uploading[DEFAULT_BUFFER_SIZE];
        size_t uploading_len = DEFAULT_BUFFER_SIZE;

        libpico_i18n("filter_downloading_active", downloading, &downloading_len);
        libpico_i18n("filter_uploading_active",   uploading,   &uploading_len);

        libpico_mainwnd_filter_add(
            mainwnd,
            filter_downloading,
            downloading,
            nullptr);

        libpico_mainwnd_filter_add(
            mainwnd,
            filter_uploading,
            uploading,
            nullptr);

        break;
    }
    }

    return libpico_ok;
}

libpico_result_t init_filters(int version, libpico_plugin_t* plugin)
{
    if (version != LIBPICO_API_VERSION)
    {
        return libpico_version_mismatch;
    }

    return libpico_register_hook(plugin, on_events, nullptr);
}

LIBPICO_DEFINE_PLUGIN(
    "filters",
    "1.0",
    init_filters);
