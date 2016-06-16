using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

using PicoTorrent.Logging;

namespace PicoTorrent.Plugins.UpdateChecker
{
    public class UpdateCheckerPlugin : IPlugin
    {
        const int MenuItemId = 10000;

        private static readonly string UpdateUrl = "https://api.github.com/repos/picotorrent/picotorrent/releases/latest";
        private readonly IPluginHost _host;

        public UpdateCheckerPlugin(IPluginHost host)
        {
            if (host == null) { throw new ArgumentNullException(nameof(host)); }
            _host = host;
        }

        public void Load()
        {
            var tr = _host.Translator.Translate("amp_check_for_update");

            _host.MainWindow.Command += (sender, args) =>
            {
                if (args.Id == MenuItemId)
                {
                    TryCheckForUpdateAsync(true);
                }
            };

            _host.MainWindow.MainMenu.Help.InsertSeparator();
            _host.MainWindow.MainMenu.Help.Insert(MenuItemId, tr);

            TryCheckForUpdateAsync();
        }

        private async void TryCheckForUpdateAsync(bool forced = false)
        {
            try
            {
                await CheckForUpdateAsync(forced);
            }
            catch (Exception e)
            {
                _host.Logger.Error("Unhandled exception when checking for update.", e);
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
                    _host.Logger.Warning("The JSON did not contain 'tag_name'.");
                    return;
                }

                var rawVersion = release["tag_name"].ToString();
                if (rawVersion.StartsWith("v")) { rawVersion = rawVersion.Substring(1); }

                var version = Version.Parse(rawVersion);
                var currentVersion = Version.Parse(_host.VersionInformation.CurrentVersion);

                if (version > currentVersion
                    && (rawVersion != _host.Configuration.IgnoredUpdate || forced))
                {
                    // A newer version is available on GitHub.
                    new UpdateAvailableDialog(_host.Configuration, _host.MainWindow, _host.Translator)
                        .Show(version, release["html_url"].ToString());
                }
                else if(forced)
                {
                    // We checked, but no new version is available. However, the user
                    // forced this from a menu click. So alert them!
                    new NoUpdateAvailableDialog(_host.MainWindow, _host.Translator)
                        .Show();
                }
            }
        }
    }
}
