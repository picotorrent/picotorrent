using System;
using System.Windows.Forms;

namespace PicoTorrent.Plugins.Pushbullet.UI
{
    public partial class ConfigControl : UserControl, IConfigControl
    {
        private class EnabledNotification
        {
            public EnabledNotification(Event e, string displayName)
            {
                Event = e;
                DisplayName = displayName;
            }

            public Event Event { get; }

            public string DisplayName { get; }

            public override string ToString()
            {
                return DisplayName;
            }
        }

        public ConfigControl()
        {
            InitializeComponent();

            _accessToken.TextChanged += (s, a) => AccessTokenChanged(this, EventArgs.Empty);
            _enabledNotificationsList.ItemCheck += (s, a) => EventsChanged(this, EventArgs.Empty);
            _testAccessToken.Click += (s, a) => TestAccessToken(this, EventArgs.Empty);

            foreach (var ev in Enum.GetNames(typeof(Event)))
            {
                Event parsed;
                if (!Enum.TryParse(ev, out parsed)) continue;
                if (parsed == Event.None) continue;

                var en = new EnabledNotification(parsed, parsed.GetDescription());
                _enabledNotificationsList.Items.Add(en);
            }
        }

        public string AccessToken
        {
            get { return _accessToken.Text; }
            set { _accessToken.Text = value; }
        }

        public event EventHandler AccessTokenChanged;

        public event EventHandler EventsChanged;

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

        public Event GetEvents()
        {
            var ev = Event.None;

            foreach (var item in _enabledNotificationsList.CheckedItems)
            {
                var en = (EnabledNotification)item;
                ev |= en.Event;
            }

            return ev;
        }

        public void SetEvents(Event e)
        {
            for (var i = 0; i < _enabledNotificationsList.Items.Count; i++)
            {
                var en = (EnabledNotification)_enabledNotificationsList.Items[i];

                if (e.HasFlag(en.Event))
                {
                    _enabledNotificationsList.SetItemChecked(i, true);
                }
            }
        }
    }
}
