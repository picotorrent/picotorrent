#include <picotorrent/common/config/configuration.hpp>

#include <picotorrent/common/environment.hpp>
#include <picotorrent/core/pal.hpp>

#include <memory>

#include <picojson.hpp>

namespace pj = picojson;
using picotorrent::common::config::configuration;
using picotorrent::common::environment;
using picotorrent::core::pal;

std::vector<std::string> configuration::plugins_part::search_paths()
{
    std::string app_path = environment::get_application_path();
    std::string default_plugins_path = pal::combine_paths(app_path, "plugins");

    std::vector<std::string> paths;
    paths.push_back(default_plugins_path);

    return paths;
}
