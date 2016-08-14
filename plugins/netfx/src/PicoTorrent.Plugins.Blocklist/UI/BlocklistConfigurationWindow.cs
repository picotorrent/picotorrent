using System;
using PicoTorrent.UI;

namespace PicoTorrent.Plugins.Blocklist.UI
{
    public sealed class BlocklistConfigurationWindow : IConfigurationWindow
    {
        private readonly IConfigControl _configControl;

        public BlocklistConfigurationWindow(IConfigControl configControl)
        {
            if (configControl == null) throw new ArgumentNullException(nameof(configControl));
            _configControl = configControl;

            // Set up events
            _configControl.BlocklistUrlChanged += (s, e) => Dirty(this, EventArgs.Empty);
            _configControl.EnableBlocklistChanged += (s, e) => Dirty(this, EventArgs.Empty);
        }

        public event EventHandler Dirty;

        public IntPtr GetHandle()
        {
            return _configControl.Handle;
        }

        public void Load()
        {
        }

        public void Save()
        {
        }
    }
}
