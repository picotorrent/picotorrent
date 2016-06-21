using System;

namespace PicoTorrent.Plugins.Pushbullet
{
    public sealed class PushbulletConfig : IPushbulletConfig
    {
        private readonly IConfiguration _config;

        public PushbulletConfig(IConfiguration config)
        {
            if (config == null) throw new ArgumentNullException(nameof(config));
            _config = config;
        }

        public string AccessToken
        {
            get { return (_config.Get<string>("pushbullet_access_token") ?? string.Empty).ToString(); }
            set { _config.Set("pushbullet_access_token", value); }
        }
    }
}
