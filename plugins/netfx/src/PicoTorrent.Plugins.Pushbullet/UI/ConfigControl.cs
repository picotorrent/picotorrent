using System;
using System.Drawing;
using System.Windows.Forms;

namespace PicoTorrent.Plugins.Pushbullet.UI
{
    public partial class ConfigControl : UserControl
    {
        public ConfigControl()
        {
            InitializeComponent();
            BackColor = Color.White;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            MessageBox.Show(this, "Hello!");
        }
    }
}
