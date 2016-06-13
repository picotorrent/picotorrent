#include "PluginHost.h"

using PicoTorrent::PluginHost;
using PicoTorrent::IVersionInformation;
using PicoTorrent::Core::ISession;
using PicoTorrent::Logging::ILogger;
using PicoTorrent::UI::IMainWindow;

PluginHost::PluginHost(
    ILogger^ logger,
    ISession^ session,
    IVersionInformation^ versionInfo,
    IMainWindow^ mainWindow)
{
    _logger = logger;
    _session = session;
    _versionInfo = versionInfo;
    _mainWindow = mainWindow;
}

ILogger^ PluginHost::Logger::get()
{
    return _logger;
}

IMainWindow^ PluginHost::MainWindow::get()
{
    return _mainWindow;
}

ISession^ PluginHost::Session::get()
{
    return _session;
}

IVersionInformation^ PluginHost::VersionInformation::get()
{
    return _versionInfo;
}
