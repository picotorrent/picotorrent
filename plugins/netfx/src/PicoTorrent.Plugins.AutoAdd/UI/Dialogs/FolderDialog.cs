using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Windows.Forms;

using PicoTorrent.Plugins.AutoAdd.Models;

namespace PicoTorrent.Plugins.AutoAdd.UI.Dialogs
{
    public partial class FolderDialog : Form
    {
        public FolderDialog(Folder f = null)
        {
            InitializeComponent();

            _browse.Click += browse_Click;
            _ok.Click += ok_Click;

            if (f != null)
            {
                _path.Text = f.Path;
                _pattern.Text = f.Pattern;
                _removeFile.Checked = f.RemoveFile;
            }
        }

        public Folder GetFolder()
        {
            return new Folder
            {
                Path = _path.Text,
                Pattern = _pattern.Text,
                RemoveFile = _removeFile.Checked
            };
        }

        private void browse_Click(object sender, EventArgs e)
        {
            var browseDialog = new FolderBrowserDialog()
            {
                ShowNewFolderButton = true
            };

            if (browseDialog.ShowDialog(this) == DialogResult.OK)
            {
                _path.Text = browseDialog.SelectedPath;
            }
        }

        private void ok_Click(object sender, EventArgs e)
        {
            if (!Directory.Exists(_path.Text))
            {
                MessageBox.Show(
                    "The specified path does not exist.",
                    "PicoTorrent",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                _path.Focus();
                return;
            }

            if (!IsValidPattern(_pattern.Text))
            {
                MessageBox.Show(
                    "Invalid regular expression.",
                    "PicoTorrent",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                _pattern.Focus();
                return;
            }

            DialogResult = DialogResult.OK;
        }

        private static bool IsValidPattern(string pattern)
        {
            try
            {
                new Regex(pattern);
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }
    }
}
