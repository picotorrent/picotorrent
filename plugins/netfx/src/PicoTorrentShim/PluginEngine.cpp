#include "PluginEngine.h"

#include "PluginHost.h"
#include "VersionInformation.h"
#include "Core/Session.h"
#include "Logging/Logger.h"
#include "UI/MainWindow.h"

using PicoTorrent::IPlugin;
using PicoTorrent::PluginEngine;
using PicoTorrent::PluginHost;
using PicoTorrent::VersionInformation;
using PicoTorrent::Core::ISession;
using PicoTorrent::Core::Session;
using PicoTorrent::Logging::Logger;
using PicoTorrent::UI::MainWindow;

PluginEngine::PluginEngine(picotorrent::core::session* sess, HWND hWnd)
{
    _host = gcnew PluginHost(
        gcnew Logger(),
        gcnew Session(sess),
        gcnew VersionInformation(),
        gcnew MainWindow(hWnd));

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


