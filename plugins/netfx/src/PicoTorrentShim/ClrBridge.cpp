#include "ClrBridge.h"

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

        return System::Reflection::Assembly::LoadFrom(dll);
    }
};

ClrBridge::ClrBridge(picotorrent::core::session* sess, HWND hWnd)
{
    System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(
        &AssemblyResolver::OnAssemblyResolve);

    _engine = gcnew PicoTorrent::PluginEngine(sess, hWnd);
}

ClrBridge::~ClrBridge()
{
}

void ClrBridge::Load()
{
    _engine->LoadAll();
}

void ClrBridge::Unload()
{
    _engine->UnloadAll();
}

