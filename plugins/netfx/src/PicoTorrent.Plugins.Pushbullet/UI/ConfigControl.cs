using System;
using System.Windows.Forms;

namespace PicoTorrent.Plugins.Pushbullet.UI
{
    public partial class ConfigControl : UserControl, IConfigControl
    {
        public ConfigControl()
        {
            InitializeComponent();
        }

        public string AccessToken
        {
            get { return _accessToken.Text; }
            set { _accessToken.Text = value; }
        }

        public event EventHandler AccessTokenChanged;

        public event EventHandler TestAccessToken;

        public void Disable()
        {
            _accessToken.Enabled = false;
            _testAccessToken.Enabled = false;
        }

        public void Enable()
        {
            _accessToken.Enabled = true;
            _testAccessToken.Enabled = true;
        }

        protected override void OnLoad(EventArgs e)
        {
            _accessToken.TextChanged += (s, a) => AccessTokenChanged(this, EventArgs.Empty);
            _testAccessToken.Click += (s, a) => TestAccessToken(this, EventArgs.Empty);
        }
    }
}
