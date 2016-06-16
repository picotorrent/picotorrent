#include "Configuration.h"

#include <msclr/marshal_cppstd.h>
#include <picotorrent/common/config/configuration.hpp>

using picotorrent::common::config::configuration;

using PicoTorrent::Config::Configuration;

System::String^ Configuration::IgnoredUpdate::get()
{
    configuration& cfg = configuration::instance();
    return msclr::interop::marshal_as<System::String^>(cfg.ignored_update());
}

void Configuration::IgnoredUpdate::set(System::String^ value)
{
    configuration& cfg = configuration::instance();
    cfg.set_ignored_update(msclr::interop::marshal_as<std::string>(value));
}
