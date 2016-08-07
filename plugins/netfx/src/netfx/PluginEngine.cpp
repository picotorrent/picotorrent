#include "PluginEngine.h"

#include <picotorrent/extensibility/plugin_host.hpp>

#include "Configuration.h"
#include "Environment.h"
#include "Translator.h"
#include "VersionInformation.h"
#include "Core/Session.h"
#include "Logging/Logger.h"
#include "UI/MainWindow.h"

using PicoTorrent::IPlugin;
using PicoTorrent::PluginEngine;

PluginEngine::PluginEngine(picotorrent::extensibility::plugin_host* host)
    : _ptr(host)
{
}

System::Collections::Generic::IList<PicoTorrent::IPlugin^>^ PluginEngine::GetAll()
{
    // Create a new container and register dependencies in it
    auto container = gcnew TinyIoC::TinyIoCContainer();
    container->Register<PicoTorrent::IEnvironment^>(gcnew PicoTorrent::Environment());
    container->Register<PicoTorrent::ITranslator^>(gcnew PicoTorrent::Translator());
    container->Register<PicoTorrent::IVersionInformation^>(gcnew PicoTorrent::VersionInformation());
    container->Register<PicoTorrent::IConfiguration^>(gcnew PicoTorrent::Configuration());
    container->Register<PicoTorrent::Core::ISession^>(gcnew PicoTorrent::Core::Session(_ptr->get_session()));
    container->Register<PicoTorrent::Logging::ILogger^>(gcnew PicoTorrent::Logging::Logger());
    container->Register<PicoTorrent::UI::IMainWindow^>(gcnew PicoTorrent::UI::MainWindow(_ptr->get_main_window()));

    // Load all plugins in the current (assembly location) directory
    auto pluginAssemblies = gcnew System::Collections::Generic::List<System::Reflection::Assembly^>();
    auto assemblyFinder = gcnew PicoTorrent::PluginAssemblyFinder();
    auto searchPath = System::IO::Path::GetDirectoryName(PluginEngine::typeid->Assembly->Location);

    auto result = gcnew System::Collections::Generic::List<PicoTorrent::IPlugin^>();

    for each (auto assembly in assemblyFinder->Find(searchPath))
    {
        auto resolver = gcnew PicoTorrent::PluginResolver();
        auto plugins = resolver->GetAll(container, assembly);
        
        for each (auto plugin in plugins)
        {
            result->Add(plugin);
        }
    }

    return result;
}
