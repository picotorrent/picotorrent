#include "PluginEngine.h"

#include <picotorrent/extensibility/plugin_host.hpp>

#include "PluginHost.h"
#include "Translator.h"
#include "VersionInformation.h"
#include "Config/Configuration.h"
#include "Core/Session.h"
#include "Logging/Logger.h"
#include "UI/MainWindow.h"

using PicoTorrent::IPlugin;
using PicoTorrent::PluginEngine;

PluginEngine::PluginEngine(picotorrent::extensibility::plugin_host* host)
{
    _host = gcnew PicoTorrent::PluginHost(
        gcnew PicoTorrent::Config::Configuration(),
        gcnew PicoTorrent::Logging::Logger(),
        gcnew PicoTorrent::Core::Session(host->get_session()),
        gcnew PicoTorrent::Translator(),
        gcnew PicoTorrent::VersionInformation(),
        gcnew PicoTorrent::UI::MainWindow(host->get_main_window()));

    _plugins = gcnew System::Collections::Generic::List<IPlugin^>();
}

void PluginEngine::LoadAll()
{
    // A hard-coded list of bundled plugins.
    auto pluginTypes = gcnew array<System::Type^>
    {
        Plugins::UpdateChecker::UpdateCheckerPlugin::typeid
    };

    for each (auto type in pluginTypes)
    {
        if (IPlugin::typeid->IsAssignableFrom(type)
            && type->IsClass
            && !type->IsAbstract)
        {
            auto ctorTypes = gcnew array<System::Type^>
            {
                IPluginHost::typeid
            };

            auto ctor = type->GetConstructor(ctorTypes);

            auto ctorParams = gcnew array<System::Object^>
            {
                _host
            };

            auto instance = (IPlugin^)ctor->Invoke(ctorParams);
            instance->Load();

            _plugins->Add(instance);
        }
    }
}

void PluginEngine::UnloadAll()
{
}


