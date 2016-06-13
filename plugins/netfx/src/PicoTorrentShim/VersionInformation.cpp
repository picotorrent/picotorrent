#include "VersionInformation.h"

#include <msclr/marshal_cppstd.h>
#include <picotorrent/core/version_info.hpp>

using picotorrent::core::version_info;
using PicoTorrent::VersionInformation;

System::String^ VersionInformation::Branch::get()
{
    return msclr::interop::marshal_as<System::String^>(version_info::git_branch());
}

System::String^ VersionInformation::Commitish::get()
{
    return msclr::interop::marshal_as<System::String^>(version_info::git_commit_hash());
}

System::String^ VersionInformation::CurrentVersion::get()
{
    return msclr::interop::marshal_as<System::String^>(version_info::current_version());
}
