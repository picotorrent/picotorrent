using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

using PicoTorrent.Logging;
using PicoTorrent.Plugins.UpdateChecker.Dialogs;
using PicoTorrent.UI;

namespace PicoTorrent.Plugins.UpdateChecker
{
    public class UpdateCheckerPlugin : IPlugin
    {
        const int MenuItemId = 10000;

        private static readonly string UpdateUrl = "https://api.github.com/repos/picotorrent/picotorrent/releases/latest";

        private readonly IUpdateCheckerConfig _config;
        private readonly IMainWindow _mainWindow;
        private readonly ITranslator _translator;
        private readonly IVersionInformation _versionInformation;
        private readonly IUpdateAvailableDialog _updateAvailableDialog;
        private readonly INoUpdateAvailableDialog _noUpdateAvailableDialog;
        private readonly ILogger _logger;

        public UpdateCheckerPlugin(IUpdateCheckerConfig config,
            IMainWindow mainWindow,
            ITranslator translator,
            IVersionInformation versionInformation,
            IUpdateAvailableDialog updateAvailableDialog,
            INoUpdateAvailableDialog noUpdateAvailableDialog,
            ILogger logger)
        {
            if (config == null) { throw new ArgumentNullException(nameof(config)); }
            if (mainWindow == null) { throw new ArgumentNullException(nameof(mainWindow)); }
            if (translator == null) { throw new ArgumentNullException(nameof(translator)); }
            if (versionInformation == null) { throw new ArgumentNullException(nameof(versionInformation)); }
            if (updateAvailableDialog == null) { throw new ArgumentNullException(nameof(updateAvailableDialog)); }
            if (noUpdateAvailableDialog == null) { throw new ArgumentNullException(nameof(noUpdateAvailableDialog)); }
            if (logger == null) { throw new ArgumentNullException(nameof(logger)); }
            _config = config;
            _mainWindow = mainWindow;
            _translator = translator;
            _versionInformation = versionInformation;
            _updateAvailableDialog = updateAvailableDialog;
            _noUpdateAvailableDialog = noUpdateAvailableDialog;
            _logger = logger;
        }

        public string Name => "UpdateChecker";

        public Version Version => typeof(UpdateCheckerPlugin).Assembly.GetName().Version;

        public IConfigurationWindow GetConfigurationWindow()
        {
            return null;
        }

        public void Load()
        {
            var tr = _translator.Translate("amp_check_for_update");

            _mainWindow.Command += (sender, args) =>
            {
                if (args.Id == MenuItemId)
                {
                    TryCheckForUpdateAsync(true);
                }
            };

            _mainWindow.MainMenu.Help.InsertSeparator();
            _mainWindow.MainMenu.Help.Insert(MenuItemId, tr);

            TryCheckForUpdateAsync();
        }

        public void Unload() { }

        private async void TryCheckForUpdateAsync(bool forced = false)
        {
            try
            {
                await CheckForUpdateAsync(forced);
            }
            catch (Exception e)
            {
                _logger.Error("Unhandled exception when checking for update.", e);
                throw;
            }
        }

        private async Task CheckForUpdateAsync(bool forced)
        {
            using (var client = new HttpClient())
            {
                client.DefaultRequestHeaders.UserAgent.Add(new ProductInfoHeaderValue("PicoTorrent", "1.0"));

                var json = await client.GetStringAsync(UpdateUrl);
                var release = new JavaScriptSerializer().Deserialize<IDictionary<string, object>>(json);
                
                if (!release.ContainsKey("tag_name"))
                {
                    _logger.Warning("The JSON did not contain 'tag_name'.");
                    return;
                }

                var rawVersion = "0.12.0"; // release["tag_name"].ToString();
                if (rawVersion.StartsWith("v")) { rawVersion = rawVersion.Substring(1); }

                var version = Version.Parse(rawVersion);
                var currentVersion = Version.Parse(_versionInformation.CurrentVersion);

                if (version > currentVersion
                    && (rawVersion != _config.IgnoredUpdate || forced))
                {
                    // A newer version is available on GitHub.
                    var url = release["html_url"].ToString();
                    _updateAvailableDialog.Show(version, url);
                }
                else if(forced)
                {
                    // We checked, but no new version is available. However, the user
                    // forced this from a menu click. So alert them!
                    _noUpdateAvailableDialog.Show();
                }
            }
        }
    }
}
