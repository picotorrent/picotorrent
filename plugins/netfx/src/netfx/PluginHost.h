#pragma once

namespace PicoTorrent
{
    ref class PluginHost : public IPluginHost
    {
    public:
        PluginHost(
            Config::IConfiguration^ configuration,
            Logging::ILogger^ logger,
            Core::ISession^ session,
            ITranslator^ translator,
            IVersionInformation^ versionInfo,
            UI::IMainWindow^ mainWindow);

        virtual property Config::IConfiguration^ Configuration { Config::IConfiguration^ get(); }

        virtual property Logging::ILogger^ Logger { Logging::ILogger^ get(); }

        virtual property UI::IMainWindow^ MainWindow { UI::IMainWindow^ get(); }

        virtual property Core::ISession^ Session { Core::ISession^ get(); }

        virtual property ITranslator^ Translator { ITranslator^ get(); }

        virtual property IVersionInformation^ VersionInformation { IVersionInformation^ get(); }

    private:
        Config::IConfiguration^ _configuration;
        Logging::ILogger^ _logger;
        Core::ISession^ _session;
        ITranslator^ _translator;
        UI::IMainWindow^ _mainWindow;
        IVersionInformation^ _versionInfo;
    };
}
