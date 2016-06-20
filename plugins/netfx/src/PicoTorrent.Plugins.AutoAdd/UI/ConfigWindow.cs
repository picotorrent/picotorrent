using System;
using PicoTorrent.UI;
using System.Windows.Forms;

namespace PicoTorrent.Plugins.AutoAdd.UI
{
    class ConfigWindow : IWindow
    {
        private readonly Button _btn = new Button();

        public IntPtr GetHandle()
        {
            _btn.Text = "AuoAdd????";
            return _btn.Handle;
        }
    }
}
