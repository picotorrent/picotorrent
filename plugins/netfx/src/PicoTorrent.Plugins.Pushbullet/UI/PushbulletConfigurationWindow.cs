using System;
using System.Threading.Tasks;
using PicoTorrent.UI;
using System.Net.Http;
using System.Text;
using System.Web.Script.Serialization;
using System.Net;
using System.Windows.Forms;

namespace PicoTorrent.Plugins.Pushbullet.UI
{
    public sealed class PushbulletConfigurationWindow : IConfigurationWindow
    {
        private readonly IPushbulletConfig _config;
        private readonly IConfigControl _control;

        public PushbulletConfigurationWindow(IPushbulletConfig config, IConfigControl configControl)
        {
            if (config == null) throw new ArgumentNullException(nameof(config));
            if (configControl == null) throw new ArgumentNullException(nameof(configControl));
            _config = config;
            _control = configControl;

            // Set up events
            _control.AccessTokenChanged += (s, a) => Dirty(this, EventArgs.Empty);
            _control.TestAccessToken += (s, a) => TestAccessToken(_control.AccessToken);
        }

        public event EventHandler Dirty;

        public IntPtr GetHandle()
        {
            return _control.Handle;
        }

        public void Load()
        {
            _control.AccessToken = _config.AccessToken;
        }

        public void Save()
        {
            _config.AccessToken = _control.AccessToken;
        }

        private async void TestAccessToken(string accessToken)
        {
            try
            {
                _control.Disable();
                var statusCode = await TestAccessTokenAsync(accessToken);
                _control.Enable();

                switch(statusCode)
                {
                    case HttpStatusCode.OK:
                        MessageBox.Show(
                            "Push sent!",
                            "PicoTorrent",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Information);
                        break;
                    case HttpStatusCode.Unauthorized:
                        MessageBox.Show(
                            "No valid access token provided (HTTP 401)",
                            "PicoTorrent",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Warning);
                        break;
                    case HttpStatusCode.Forbidden:
                        MessageBox.Show(
                            "The access token is not valid for this request (HTTP 403)",
                            "PicoTorrent",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Warning);
                        break;
                }
            }
            catch (Exception)
            {
                throw;
            }
        }

        private async Task<HttpStatusCode> TestAccessTokenAsync(string accessToken)
        {
            using (var client = new HttpClient())
            {
                client.DefaultRequestHeaders.Add("Access-Token", accessToken);

                var push = new
                {
                    body = "A test notification from PicoTorrent.",
                    title = "PicoTorrent",
                    type = "note"
                };

                var json = new JavaScriptSerializer().Serialize(push);
                var content = new StringContent(json, Encoding.UTF8, "application/json");
                var response = await client.PostAsync("https://api.pushbullet.com/v2/pushes", content);

                return response.StatusCode;
            }
        }
    }
}
