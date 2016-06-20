#include "ClrPluginConfigWindow.h"

using PicoTorrent::UI::IConfigurationWindow;

ClrPluginConfigWindow::ClrPluginConfigWindow(gcroot<IConfigurationWindow^> instance)
    : _instance(instance)
{
}

HWND ClrPluginConfigWindow::handle()
{
    return static_cast<HWND>(_instance->GetHandle().ToPointer());
}

void ClrPluginConfigWindow::load()
{
    _instance->Load();
}

void ClrPluginConfigWindow::save()
{
    _instance->Save();
}
