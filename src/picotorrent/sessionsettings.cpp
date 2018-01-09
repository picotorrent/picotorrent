#include "sessionsettings.hpp"

#include <libtorrent/alert.hpp>
#include <libtorrent/settings_pack.hpp>

#include <iomanip>
#include <sstream>

#include "buildinfo.hpp"
#include "config.hpp"
#include "semver.hpp"

namespace lt = libtorrent;
using pt::SessionSettings;

lt::settings_pack SessionSettings::Get(std::shared_ptr<pt::Configuration> cfg)
{
    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings.set_str(lt::settings_pack::string_types::dht_bootstrap_nodes,
        "router.bittorrent.com:6881" ","
        "router.utorrent.com:6881" ","
        "dht.transmissionbt.com:6881" ","
        "dht.aelitis.com:6881");

    std::stringstream ifaces;
    for (auto &p : cfg->ListenInterfaces())
    {
        ifaces << "," << p.first << ":" << p.second;
    }

    // Features
    settings.set_bool(lt::settings_pack::enable_dht, cfg->Session()->EnableDht());
    settings.set_bool(lt::settings_pack::enable_lsd, cfg->Session()->EnableLsd());

    // Limits
    settings.set_int(lt::settings_pack::active_checking, cfg->Session()->ActiveChecking());
    settings.set_int(lt::settings_pack::active_dht_limit, cfg->Session()->ActiveDhtLimit());
    settings.set_int(lt::settings_pack::active_downloads, cfg->Session()->ActiveDownloads());
    settings.set_int(lt::settings_pack::active_limit, cfg->Session()->ActiveLimit());
    settings.set_int(lt::settings_pack::active_lsd_limit, cfg->Session()->ActiveLsdLimit());
    settings.set_int(lt::settings_pack::active_seeds, cfg->Session()->ActiveSeeds());
    settings.set_int(lt::settings_pack::active_tracker_limit, cfg->Session()->ActiveTrackerLimit());

    // Encryption
    lt::settings_pack::enc_policy in_policy = cfg->Session()->RequireIncomingEncryption()
        ? lt::settings_pack::enc_policy::pe_forced
        : lt::settings_pack::enc_policy::pe_enabled;

    lt::settings_pack::enc_policy out_policy = cfg->Session()->RequireOutgoingEncryption()
        ? lt::settings_pack::enc_policy::pe_forced
        : lt::settings_pack::enc_policy::pe_enabled;

    settings.set_int(lt::settings_pack::int_types::in_enc_policy, in_policy);
    settings.set_int(lt::settings_pack::int_types::out_enc_policy, out_policy);

    // Various
    settings.set_bool(lt::settings_pack::anonymous_mode, cfg->Session()->EnableAnonymousMode());
    settings.set_str(lt::settings_pack::listen_interfaces, ifaces.str().substr(1));
    settings.set_int(lt::settings_pack::download_rate_limit, cfg->Session()->DownloadRateLimit());
    settings.set_int(lt::settings_pack::stop_tracker_timeout, cfg->Session()->StopTrackerTimeout());
    settings.set_int(lt::settings_pack::upload_rate_limit, cfg->Session()->UploadRateLimit());

    // Calculate user agent
    std::stringstream user_agent;
    user_agent << "PicoTorrent/" << BuildInfo::Version();

    // Calculate peer id
    semver::version v(BuildInfo::Version());
    std::stringstream peer_id;
    peer_id << "-PI" << v.getMajor() << std::setfill('0') << std::setw(2) << v.getMinor() << v.getPatch() << "-";

    settings.set_str(lt::settings_pack::user_agent, user_agent.str());
    settings.set_str(lt::settings_pack::peer_fingerprint, peer_id.str());

    // Proxy settings
    settings.set_int(lt::settings_pack::proxy_type, static_cast<lt::settings_pack::proxy_type_t>(cfg->ProxyType()));
    settings.set_str(lt::settings_pack::proxy_hostname, cfg->ProxyHost());
    settings.set_int(lt::settings_pack::proxy_port, cfg->ProxyPort());
    settings.set_str(lt::settings_pack::proxy_username, cfg->ProxyUsername());
    settings.set_str(lt::settings_pack::proxy_password, cfg->ProxyPassword());
    settings.set_bool(lt::settings_pack::force_proxy, cfg->ProxyForce());
    settings.set_bool(lt::settings_pack::proxy_hostnames, cfg->ProxyHostnames());
    settings.set_bool(lt::settings_pack::proxy_peer_connections, cfg->ProxyPeers());
    settings.set_bool(lt::settings_pack::proxy_tracker_connections, cfg->ProxyTrackers());

    return settings;
}
