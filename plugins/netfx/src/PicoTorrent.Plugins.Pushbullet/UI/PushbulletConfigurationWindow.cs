using System;
using System.Windows.Forms;
using PicoTorrent.Plugins.Pushbullet.Net;
using PicoTorrent.UI;

namespace PicoTorrent.Plugins.Pushbullet.UI
{
    public sealed class PushbulletConfigurationWindow : IConfigurationWindow
    {
        private readonly IPushbulletConfig _config;
        private readonly IConfigControl _control;
        private readonly IPushbulletClient _client;

        public PushbulletConfigurationWindow(IPushbulletConfig config, IConfigControl configControl, IPushbulletClient client)
        {
            if (config == null) throw new ArgumentNullException(nameof(config));
            if (configControl == null) throw new ArgumentNullException(nameof(configControl));
            if (client == null) throw new ArgumentNullException(nameof(client));

            _config = config;
            _control = configControl;
            _client = client;

            // Set up events
            _control.AccessTokenChanged += (s, a) => Dirty(this, EventArgs.Empty);
            _control.EventsChanged += (s, a) => Dirty(this, EventArgs.Empty);
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
            _control.SetEvents(_config.Events);
        }

        public void Save()
        {
            _config.AccessToken = _control.AccessToken;
            _config.Events = _control.GetEvents();
        }

        private async void TestAccessToken(string accessToken)
        {
            _control.Disable();

            try
            {
                await _client.PushNoteAsync(
                    "PicoTorrent",
                    "Testing Pushbullet from PicoTorrent.",
                    accessToken);

                MessageBox.Show(
                    "Push sent!",
                    "PicoTorrent",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
            catch (PushbulletHttpException ex)
            {
                var msg = $"An error occured when calling Pushbullet.{Environment.NewLine}{Environment.NewLine}{ex.Message}";

                MessageBox.Show(
                    msg,
                    "PicoTorrent",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
            }
            finally
            {
                _control.Enable();
            }
        }
    }
}
