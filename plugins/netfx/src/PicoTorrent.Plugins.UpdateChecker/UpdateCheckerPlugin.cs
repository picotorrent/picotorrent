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
        private static readonly string UpdateUrl = "https://api.github.com/repos/picotorrent/picotorrent/releases/latest";
        private readonly IPluginHost _host;

        public UpdateCheckerPlugin(IPluginHost host)
        {
            if (host == null) { throw new ArgumentNullException(nameof(host)); }
            _host = host;
        }

        public void Load()
        {
            LoadAsync();
        }

        private async void LoadAsync()
        {
            try
            {
                await CheckForUpdateAsync();
            }
            catch(Exception e)
            {
                _host.Logger.Error("Unhandled exception when checking for update.", e);
                throw;
            }
        }
        
        private async Task CheckForUpdateAsync()
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

                _host.MainWindow.ShowMessageBox("Version", rawVersion);

                if (version <= currentVersion)
                {
                    _host.Logger.Information("Current version is greater than version from GitHub.");
                    return;
                }

                // A newer version is available on GitHub. Alert da user.
            }
        }
    }
}
