#include "ClrPlugin.h"

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

HWND ClrPlugin::get_window()
{
    auto wnd = _instance->GetWindow();
    if (wnd == nullptr) { return NULL; }
    return static_cast<HWND>(wnd->GetHandle().ToPointer());
}

void ClrPlugin::load()
{
    _instance->Load();
}

void ClrPlugin::unload()
{
    _instance->Unload();
}
