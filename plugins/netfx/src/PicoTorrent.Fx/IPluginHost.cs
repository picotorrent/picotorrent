using PicoTorrent.Config;
using PicoTorrent.Core;
using PicoTorrent.Logging;
using PicoTorrent.UI;

namespace PicoTorrent
{
    public interface IPluginHost
    {
        IConfiguration Configuration { get;}

        ILogger Logger { get; }

        IMainWindow MainWindow { get; }

        ISession Session { get; }

        ITranslator Translator { get; }

        IVersionInformation VersionInformation { get; }
    }
}
