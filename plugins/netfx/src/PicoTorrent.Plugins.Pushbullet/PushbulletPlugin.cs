using PicoTorrent.Core;
using PicoTorrent.Logging;
using PicoTorrent.Plugins.Pushbullet.Net;
using PicoTorrent.UI;
using System;

namespace PicoTorrent.Plugins.Pushbullet
{
    public class PushbulletPlugin : Plugin
    {
        private readonly ILogger _logger;
        private readonly IPushbulletConfig _config;
        private readonly IPushbulletClient _client;
        private readonly IConfigurationWindow _configWindow;
        private readonly ISession _session;

        public PushbulletPlugin(
            ILogger logger,
            IPushbulletConfig config,
            IPushbulletClient client,
            IConfigurationWindow configWindow,
            ISession session)
        {
            if (logger == null) throw new ArgumentNullException(nameof(logger));
            if (config == null) throw new ArgumentNullException(nameof(config));
            if (client == null) throw new ArgumentNullException(nameof(client));
            if (configWindow == null) throw new ArgumentNullException(nameof(configWindow));
            if (session == null) throw new ArgumentNullException(nameof(session));

            _logger = logger;
            _config = config;
            _client = client;
            _configWindow = configWindow;
            _session = session;

            _session.TorrentAdded += (sender, args) => SendNotification(Event.TorrentAdded, args.Torrent);
            _session.TorrentFinished += (sender, args) => SendNotification(Event.TorrentFinished, args.Torrent);
            _session.TorrentRemoved += (sender, args) => SendNotification(Event.TorrentRemoved, args.Torrent);
        }

        public override IConfigurationWindow GetConfigurationWindow()
        {
            return _configWindow;
        }

        public override void Load()
        {
        }

        private async void SendNotification(Event ev, ITorrent torrent)
        {
            if (!_config.Events.HasFlag(ev))
            {
                return;
            }

            var msg = string.Empty;

            switch(ev)
            {
                case Event.TorrentAdded:
                    msg = $"Torrent '{torrent.Name}' added.";
                    break;
                case Event.TorrentFinished:
                    msg = $"Torrent '{torrent.Name}' finished.";
                    break;
                case Event.TorrentRemoved:
                    msg = $"Torrent '{torrent.Name}' removed.";
                    break;
            }

            if (string.IsNullOrEmpty(msg))
            {
                return;
            }

            try
            {
                await _client.PushNoteAsync("PicoTorrent", msg);
            }
            catch (Exception ex)
            {
                _logger.Error("Could not send Pushbullet notification.", ex);
            }
        }
    }
}
