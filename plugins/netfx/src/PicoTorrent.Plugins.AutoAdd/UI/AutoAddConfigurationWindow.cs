using System;
using PicoTorrent.UI;

namespace PicoTorrent.Plugins.AutoAdd.UI
{
    public sealed class AutoAddConfigurationWindow : IConfigurationWindow
    {
        private readonly IAutoAddConfig _config;
        private readonly IConfigControl _control;

        public AutoAddConfigurationWindow(IAutoAddConfig config, IConfigControl control)
        {
            if (config == null) throw new ArgumentNullException(nameof(config));
            if (control == null) throw new ArgumentNullException(nameof(control));
            _config = config;
            _control = control;

            _control.FoldersChanged += (sender, args) => Dirty(this, EventArgs.Empty);
        }

        public event EventHandler Dirty;

        public IntPtr GetHandle()
        {
            return _control.Handle;
        }

        public void Load()
        {
            _control.SetFolders(_config.Folders);
        }

        public void Save()
        {
            _config.Folders = _control.GetFolders();
        }
    }
}
