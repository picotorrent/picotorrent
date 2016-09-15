#include "SessionSettings.hpp"

#include <libtorrent/alert_types.hpp>
#include <libtorrent/settings_pack.hpp>
#include <semver.hpp>

#include "../Configuration.hpp"
#include "../VersionInformation.hpp"

namespace lt = libtorrent;
using Core::SessionSettings;

lt::settings_pack SessionSettings::Get()
{
    Configuration& cfg = Configuration::GetInstance();

    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::category_t::all_categories);

    std::stringstream ifaces;
    for (auto &p : cfg.GetListenInterfaces())
    {
        ifaces << "," << p.first << ":" << p.second;
    }

    // Limits
    settings.set_int(lt::settings_pack::active_checking, cfg.Session()->GetActiveChecking());
    settings.set_int(lt::settings_pack::active_dht_limit, cfg.Session()->GetActiveDhtLimit());
    settings.set_int(lt::settings_pack::active_downloads, cfg.Session()->GetActiveDownloads());
    settings.set_int(lt::settings_pack::active_limit, cfg.Session()->GetActiveLimit());
    settings.set_int(lt::settings_pack::active_loaded_limit, cfg.Session()->GetActiveLoadedLimit());
    settings.set_int(lt::settings_pack::active_lsd_limit, cfg.Session()->GetActiveLsdLimit());
    settings.set_int(lt::settings_pack::active_seeds, cfg.Session()->GetActiveSeeds());
    settings.set_int(lt::settings_pack::active_tracker_limit, cfg.Session()->GetActiveTrackerLimit());

    // Various
    settings.set_str(lt::settings_pack::listen_interfaces, ifaces.str().substr(1));
    settings.set_int(lt::settings_pack::download_rate_limit, cfg.Session()->GetDownloadRateLimit());
    settings.set_bool(lt::settings_pack::enable_dht, true);
    // listen interfaces
    settings.set_int(lt::settings_pack::stop_tracker_timeout, cfg.Session()->GetStopTrackerTimeout());
    settings.set_int(lt::settings_pack::upload_rate_limit, cfg.Session()->GetUploadRateLimit());

    // Calculate user agent
    std::stringstream user_agent;
    user_agent << "PicoTorrent/" << VersionInformation::GetCurrentVersion();

    // Calculate peer id
    semver::version v(VersionInformation::GetCurrentVersion());
    std::stringstream peer_id;
    peer_id << "-PI" << v.getMajor() << std::setfill('0') << std::setw(2) << v.getMinor() << v.getPatch() << "-";

    settings.set_str(lt::settings_pack::user_agent, user_agent.str());
    settings.set_str(lt::settings_pack::peer_fingerprint, peer_id.str());

    // Proxy settings
    settings.set_int(lt::settings_pack::proxy_type, (int)cfg.GetProxyType());
    settings.set_str(lt::settings_pack::proxy_hostname, cfg.GetProxyHost());
    settings.set_int(lt::settings_pack::proxy_port, cfg.GetProxyPort());
    settings.set_bool(lt::settings_pack::force_proxy, cfg.GetProxyForce());
    settings.set_bool(lt::settings_pack::proxy_hostnames, cfg.GetProxyHostnames());
    settings.set_bool(lt::settings_pack::proxy_peer_connections, cfg.GetProxyPeers());
    settings.set_bool(lt::settings_pack::proxy_tracker_connections, cfg.GetProxyTrackers());

    return settings;
}
