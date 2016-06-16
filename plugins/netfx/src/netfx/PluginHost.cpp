#include "PluginHost.h"

using PicoTorrent::PluginHost;
using PicoTorrent::ITranslator;
using PicoTorrent::IVersionInformation;
using PicoTorrent::Config::IConfiguration;
using PicoTorrent::Core::ISession;
using PicoTorrent::Logging::ILogger;
using PicoTorrent::UI::IMainWindow;

PluginHost::PluginHost(
    IConfiguration^ configuration,
    ILogger^ logger,
    ISession^ session,
    ITranslator^ translator,
    IVersionInformation^ versionInfo,
    IMainWindow^ mainWindow)
{
    _configuration = configuration;
    _logger = logger;
    _session = session;
    _translator = translator;
    _versionInfo = versionInfo;
    _mainWindow = mainWindow;
}

IConfiguration^ PluginHost::Configuration::get()
{
    return _configuration;
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

ITranslator^ PluginHost::Translator::get()
{
    return _translator;
}

IVersionInformation^ PluginHost::VersionInformation::get()
{
    return _versionInfo;
}
