namespace PicoTorrent.Plugins.Pushbullet
{
    public sealed class PushbulletConfig : ConfigurationPart, IPushbulletConfig
    {
        public PushbulletConfig(IConfiguration config)
            : base(config, "pushbullet")
        {
        }

        public string AccessToken
        {
            get { return Get("access_token", string.Empty); }
            set { Set("access_token", value); }
        }

        public Event Events
        {
            get { return Get("enabled_events", Event.None); }
            set { Set("enabled_events", value); }
        }
    }
}
