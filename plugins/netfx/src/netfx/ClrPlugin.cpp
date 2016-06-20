#include "ClrPlugin.h"

#include "ClrPluginConfigWindow.h"

#include <msclr/marshal_cppstd.h>

ClrPlugin::ClrPlugin(gcroot<PicoTorrent::IPlugin^> instance)
    : _instance(instance)
{
}

std::string ClrPlugin::get_name()
{
    return msclr::interop::marshal_as<std::string>(_instance->Name);
}

std::string ClrPlugin::get_version()
{
    return msclr::interop::marshal_as<std::string>(_instance->Version->ToString(3));
}

std::shared_ptr<picotorrent::plugin_config_window> ClrPlugin::get_config_window()
{
    auto wnd = _instance->GetConfigurationWindow();
    if (wnd == nullptr) { return nullptr; }

    return std::shared_ptr<ClrPluginConfigWindow>(new ClrPluginConfigWindow(wnd));
}

void ClrPlugin::load()
{
    _instance->Load();
}

void ClrPlugin::unload()
{
    _instance->Unload();
}
