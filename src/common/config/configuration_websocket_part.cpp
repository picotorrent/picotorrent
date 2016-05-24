#include <picotorrent/common/config/configuration.hpp>

#include <picotorrent/common/environment.hpp>
#include <picotorrent/core/pal.hpp>

#include <memory>

#include <picojson.hpp>

namespace pj = picojson;
using picotorrent::common::config::configuration;
using picotorrent::common::environment;
using picotorrent::core::pal;

std::string configuration::websocket_part::access_token()
{
    return get_part_key_or_default("websocket", "access_token", std::string());
}

void configuration::websocket_part::access_token(const std::string &token)
{
    set_part_key("websocket", "access_token", token);
}

std::string configuration::websocket_part::certificate_file()
{
    std::string data_path = environment::get_data_path();
    std::string default_file = pal::combine_paths(data_path, "PicoTorrent_generated.pem");

    return get_part_key_or_default("websocket", "certificate_file", default_file);
}

std::string configuration::websocket_part::certificate_password()
{
    return get_part_key_or_default("websocket", "certificate_password", std::string());
}

std::string configuration::websocket_part::cipher_list()
{
    std::string default_ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK";
    return get_part_key_or_default("websocket", "certificate_password", default_ciphers);
}

bool configuration::websocket_part::enabled()
{
    return get_part_key_or_default("websocket", "enabled", false);
}

void configuration::websocket_part::enabled(bool enable)
{
    set_part_key("websocket", "enabled", enable);
}

int configuration::websocket_part::listen_port()
{
    return get_part_key_or_default("websocket", "listen_port", 7676);
}

void configuration::websocket_part::listen_port(int port)
{
    set_part_key("websocket", "listen_port", port);
}
