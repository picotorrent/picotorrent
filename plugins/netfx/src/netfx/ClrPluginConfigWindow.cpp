#include "ClrPluginConfigWindow.h"

using PicoTorrent::UI::IConfigurationWindow;

ref class EventHandlerWrapper
{
public:
    EventHandlerWrapper(ClrPluginConfigWindow* wnd)
        : _wnd(wnd)
    {
    }

    void OnDirty(System::Object^ sender, System::EventArgs^ args)
    {
        _wnd->raise_dirty();
    }

private:
    ClrPluginConfigWindow* _wnd;
};

ClrPluginConfigWindow::ClrPluginConfigWindow(gcroot<IConfigurationWindow^> instance)
    : _instance(instance),
    _eventWrapper(gcnew EventHandlerWrapper(this))
{
    _instance->Dirty += gcnew System::EventHandler(_eventWrapper, &EventHandlerWrapper::OnDirty);
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

void ClrPluginConfigWindow::raise_dirty()
{
    emit_dirty();
}
