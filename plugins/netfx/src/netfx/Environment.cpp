#include "Environment.h"

#include <msclr/marshal_cppstd.h>
#include <picotorrent/common/environment.hpp>

using picotorrent::common::environment;

using PicoTorrent::Environment;

System::String^ Environment::ApplicationPath::get()
{
    return msclr::interop::marshal_as<System::String^>(environment::get_application_path());
}

System::String^ Environment::DataPath::get()
{
    return msclr::interop::marshal_as<System::String^>(environment::get_data_path());
}

bool Environment::IsInstalled::get()
{
    return environment::is_installed();
}
