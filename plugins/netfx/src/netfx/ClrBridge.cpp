#include "ClrBridge.h"

#include "ClrPlugin.h"
#include "PluginEngine.h"

ref class AssemblyResolver
{
public:
    static System::Reflection::Assembly^ OnAssemblyResolve(System::Object^ sender, System::ResolveEventArgs^ args)
    {
        System::String^ name = args->Name;
        if (name->Contains(",")) { name = name->Substring(0, name->IndexOf(",")); }

        System::String^ location = System::Reflection::Assembly::GetExecutingAssembly()->Location;
        System::String^ baseName = System::IO::Path::GetDirectoryName(location);
        System::String^ dll = System::IO::Path::Combine(baseName, name + ".dll");

        if (!System::IO::File::Exists(dll))
        {
            return nullptr;
        }

        return System::Reflection::Assembly::LoadFrom(dll);
    }
};

ClrBridge::ClrBridge(picotorrent::extensibility::plugin_host* host)
{
    System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(
        &AssemblyResolver::OnAssemblyResolve);

    _engine = gcnew PicoTorrent::PluginEngine(host);
}

std::vector<picotorrent::plugin_ptr> ClrBridge::GetPlugins()
{
    std::vector<picotorrent::plugin_ptr> plugins;

    for each (auto plugin in _engine->GetAll())
    {
        std::shared_ptr<picotorrent::plugin> p(new ClrPlugin(plugin));
        plugins.push_back(p);
    }

    return plugins;
}
