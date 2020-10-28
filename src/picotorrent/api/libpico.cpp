#include "libpico_impl.hpp"

#ifdef _WIN32
#include <Windows.h>

#include <vector>

#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "../buildinfo.hpp"
#include "../core/configuration.hpp"
#include "../core/environment.hpp"
#include "../core/utils.hpp"
#include "../http/httpclient.hpp"
#include "../ui/mainframe.hpp"
#include "../ui/translator.hpp"

using pt::API::IPlugin;

struct libpico_http_response_t
{
    std::string data;
};

class Plugin : public IPlugin
{
public:
    Plugin(HMODULE hModule, pt::Core::Environment* env, pt::Core::Configuration* cfg)
        : m_hModule(hModule),
        m_env(env),
        m_cfg(cfg)
    {
        libpico_init_t init = reinterpret_cast<libpico_init_t>(
            GetProcAddress(
                m_hModule,
                LIBPICO_TOSTRING(LIBPICO_INIT)));

        init(LIBPICO_API_VERSION,
            reinterpret_cast<libpico_plugin_t*>(this));
    }

    virtual ~Plugin()
    {
        EmitEvent(libpico_event_shutdown, nullptr);
    }

    pt::Core::Configuration* Configuration() { return m_cfg; }
    pt::Core::Environment* Environment() { return m_env; }

    void AddHook(libpico_hook_callback_t callback, libpico_param_t* param)
    {
        m_hooks.push_back({ callback, param });
    }

    void EmitEvent(libpico_event_t event, void* param)
    {
        for (auto const& cb : m_hooks)
        {
            cb.callback(event, reinterpret_cast<libpico_param_t*>(param), cb.param);
        }
    }

private:
    struct Hook
    {
        libpico_hook_callback_t callback;
        libpico_param_t* param;
    };

    pt::Core::Environment* m_env;
    pt::Core::Configuration* m_cfg;
    HMODULE m_hModule;
    std::vector<Hook> m_hooks;
};

IPlugin::~IPlugin()
{
}

IPlugin* IPlugin::Load(fs::path const& p, Core::Environment* env, Core::Configuration* cfg)
{
    return new Plugin(
        LoadLibrary(p.wstring().c_str()),
        env,
        cfg);
}

libpico_result_t libpico_config_get(libpico_plugin_t* plugin, libpico_config_t** config)
{
    auto p = reinterpret_cast<Plugin*>(plugin);
    *config = reinterpret_cast<libpico_config_t*>(p->Configuration());
    return libpico_ok;
}

libpico_result_t libpico_config_bool_get(libpico_config_t* cfg, const char* key, bool* result)
{
    auto config = reinterpret_cast<pt::Core::Configuration*>(cfg);
    *result = config->Get<bool>(key).value();
    return libpico_ok;
}

libpico_result_t libpico_config_string_get(libpico_config_t* cfg, const char* key, char* result, size_t* len)
{
    auto config = reinterpret_cast<pt::Core::Configuration*>(cfg);
    auto res = config->Get<std::string>(key).value_or("");
    strncpy(result, res.c_str(), *len);
    *len = res.size();
    return libpico_ok;
}

libpico_result_t libpico_config_string_set(libpico_config_t* config, const char* key, const char* value, size_t len)
{
    std::string val = len == (size_t)-1
        ? std::string(value)
        : std::string(value, len);

    reinterpret_cast<pt::Core::Configuration*>(config)->Set(key, val);

    return libpico_ok;
}

libpico_result_t libpico_http_get(const char* url, libpico_http_callback_t callback, libpico_param_t* user)
{
    auto client = std::make_shared<pt::Http::HttpClient>();
    client->Get(url, [client, callback, user](int statusCode, std::string const& data)
        {
            libpico_http_response_t response;
            response.data = data;
            callback(&response, (libpico_http_status_t)statusCode, user);
        });

    return libpico_ok;
}

libpico_result_t libpico_http_response_body(libpico_http_response_t* response, char* body, size_t len)
{
    strncpy(body, response->data.c_str(), len);
    return libpico_ok;
}

libpico_result_t libpico_http_response_body_len(libpico_http_response_t* response, size_t* len)
{
    *len = response->data.size();
    return libpico_ok;
}

libpico_result_t libpico_i18n(const char* key, wchar_t* target, size_t* len)
{
    wxString translation = pt::UI::Translator::GetInstance().Translate(key);
    size_t ll = len == nullptr
        ? wcslen(target)
        : *len;

    if (len != nullptr)
    {
        *len = translation.size();
    }

    if (translation.size() > ll || target == nullptr)
    {
        return libpico_insufficient_buffer;
    }

    wcsncpy(target, translation.wc_str(), ll);

    return libpico_ok;
}

libpico_result_t libpico_mainwnd_filter_add(libpico_mainwnd_t* wnd, libpico_filter_callback_t cb, const wchar_t* name, libpico_param_t* user)
{
    reinterpret_cast<pt::UI::MainFrame*>(wnd)->AddFilter(
        name,
        [cb, user](pt::BitTorrent::TorrentHandle* torrent) -> bool
        {
            return cb(
                reinterpret_cast<libpico_torrent_t*>(torrent),
                user);
        });

    return libpico_ok;
}

libpico_result_t libpico_mainwnd_native_handle(libpico_mainwnd_t* wnd, void** handle)
{
    *handle = reinterpret_cast<pt::UI::MainFrame*>(wnd)->GetHWND();
    return libpico_ok;
}

libpico_result_t libpico_menu_get(libpico_mainwnd_t* wnd, libpico_menu_id_t id, libpico_menu_t** menu)
{
    auto mf = reinterpret_cast<pt::UI::MainFrame*>(wnd);
    auto menuBar = mf->GetMenuBar();

    switch (id)
    {
    case libpico_menu_help:
        *menu = reinterpret_cast<libpico_menu_t*>(menuBar->GetMenu(2));
        return libpico_ok;
    }

    return libpico_err;
}

libpico_result_t libpico_menu_insert_item(libpico_menu_t* menu, uint32_t pos, const wchar_t* label, size_t len, libpico_menuitem_callback_t cb, libpico_param_t* user, libpico_menuitem_t** item)
{
    auto m = reinterpret_cast<wxMenu*>(menu);
    auto itm = m->Insert(pos, 9999, wxString(label, len));
    m->Bind(
        wxEVT_MENU,
        [menu, item, cb, user](wxCommandEvent&)
        {
            cb(nullptr, user);
        },
        9999);

    if (item != nullptr)
    {
        *item = reinterpret_cast<libpico_menuitem_t*>(itm);
    }

    return libpico_ok;
}

libpico_result_t libpico_menu_insert_separator(libpico_menu_t* menu, uint32_t pos)
{
    auto m = reinterpret_cast<wxMenu*>(menu);
    m->InsertSeparator(pos);
    return libpico_ok;
}

libpico_result_t libpico_register_hook(libpico_plugin_t* plugin, libpico_hook_callback_t cb, libpico_param_t* user)
{
    reinterpret_cast<Plugin*>(plugin)->AddHook(cb, user);
    return libpico_ok;
}

libpico_result_t libpico_string_towide(const char* input, wchar_t* output, size_t len)
{
    auto out = pt::Utils::toStdWString(input);
    wcsncpy(output, out.c_str(), len);
    return libpico_ok;
}

libpico_result_t libpico_torrent_stats_get(libpico_torrent_t* torrent, libpico_torrent_stats_t* stats)
{
    auto t = reinterpret_cast<pt::BitTorrent::TorrentHandle*>(torrent);
    auto s = t->Status();

    stats->download_payload_rate = s.downloadPayloadRate;
    stats->upload_payload_rate = s.uploadPayloadRate;

    return libpico_ok;
}

const char* libpico_version()
{
    return pt::BuildInfo::version();
}
#else


pt::API::IPlugin* pt::API::IPlugin::Load(fs::path const& p, Core::Environment* env, Core::Configuration* cfg)
{
    return nullptr;
}

#endif
