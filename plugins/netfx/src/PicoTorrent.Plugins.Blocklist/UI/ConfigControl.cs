using System;
using System.Windows.Forms;

namespace PicoTorrent.Plugins.Blocklist.UI
{
    public partial class ConfigControl : UserControl, IConfigControl
    {
        public ConfigControl()
        {
            InitializeComponent();

            _blocklistUrl.TextChanged += (s, e) => BlocklistUrlChanged(this, EventArgs.Empty);
            _enableBlocklist.CheckedChanged += (s, e) => EnableBlocklistChanged(this, EventArgs.Empty);
        }

        public event EventHandler BlocklistUrlChanged;

        public event EventHandler EnableBlocklistChanged;

        public string BlocklistUrl
        {
            get { return _blocklistUrl.Text; }
            set { _blocklistUrl.Text = value; }
        }

        public bool EnableBlocklist
        {
            get { return _enableBlocklist.Checked; }
            set { _enableBlocklist.Checked = value; }
        }
    }
}
