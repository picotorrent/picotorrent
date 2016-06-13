#pragma once

namespace PicoTorrent
{
    ref class PluginHost : public IPluginHost
    {
    public:
        PluginHost(
            Logging::ILogger^ logger,
            Core::ISession^ session,
            IVersionInformation^ versionInfo,
            UI::IMainWindow^ mainWindow);

        virtual property Logging::ILogger^ Logger { Logging::ILogger^ get(); }

        virtual property UI::IMainWindow^ MainWindow { UI::IMainWindow^ get(); }

        virtual property Core::ISession^ Session { Core::ISession^ get(); }

        virtual property IVersionInformation^ VersionInformation { IVersionInformation^ get(); }

    private:
        Logging::ILogger^ _logger;
        Core::ISession^ _session;
        UI::IMainWindow^ _mainWindow;
        IVersionInformation^ _versionInfo;
    };
}
