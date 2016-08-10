using System;

namespace PicoTorrent.Plugins.UpdateChecker
{
    public sealed class UpdateCheckerConfig : IUpdateCheckerConfig
    {
        private readonly IConfiguration _config;

        public UpdateCheckerConfig(IConfiguration config)
        {
            if (config == null) throw new ArgumentNullException(nameof(config));
            _config = config;
        }

        public string IgnoredUpdate
        {
            get { return (_config.Get<string>("ignored_update") ?? string.Empty).ToString(); }
            set { _config.Set("ignored_update", value); }
        }
    }
}
