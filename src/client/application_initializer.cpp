#include <picotorrent/client/application_initializer.hpp>

#include <fstream>
#include <string>
#include <vector>

#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/environment.hpp>
#include <picotorrent/client/security/certificate_manager.hpp>
#include <picotorrent/client/security/random_string_generator.hpp>
#include <picotorrent/core/pal.hpp>

#define DEFAULT_ACCESS_TOKEN_SIZE 20

using picotorrent::client::application_initializer;
using picotorrent::client::configuration;
using picotorrent::client::environment;
using picotorrent::client::security::certificate_manager;
using picotorrent::client::security::random_string_generator;
using picotorrent::core::pal;

void application_initializer::create_application_paths()
{
    auto create_if_not_exists = [](const std::string &path)
    {
        if (!pal::directory_exists(path))
        {
            pal::create_directories(path);
        }
    };

    create_if_not_exists(environment::get_data_path());
    create_if_not_exists(pal::combine_paths(environment::get_data_path(), "Torrents"));
}

void application_initializer::generate_websocket_access_token()
{
    configuration &cfg = configuration::instance();
    std::string access_token = cfg.websocket_access_token();

    if (access_token.empty())
    {
        random_string_generator rsg;
        std::string random_token = rsg.generate(DEFAULT_ACCESS_TOKEN_SIZE);
        cfg.set_websocket_access_token(random_token);
    }
}

void application_initializer::generate_websocket_certificate()
{
    configuration &cfg = configuration::instance();
    std::string certificate_file = cfg.websocket_certificate_file();

    if (!pal::file_exists(certificate_file))
    {
        auto v = certificate_manager::generate();
        std::ofstream co(certificate_file, std::ios::binary);
        co.write(&v[0], v.size());
    }
}
