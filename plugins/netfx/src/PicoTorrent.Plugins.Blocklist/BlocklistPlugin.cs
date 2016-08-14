using PicoTorrent.UI;
using System;

namespace PicoTorrent.Plugins.Blocklist
{
    public sealed class BlocklistPlugin : Plugin
    {
        private readonly IConfigurationWindow _configWindow;

        public BlocklistPlugin(IConfigurationWindow configWindow)
        {
            if (configWindow == null) throw new ArgumentNullException(nameof(configWindow));
            _configWindow = configWindow;
        }

        public override IConfigurationWindow GetConfigurationWindow()
        {
            return _configWindow;
        }

        public override void Load()
        {
        }
    }
}
