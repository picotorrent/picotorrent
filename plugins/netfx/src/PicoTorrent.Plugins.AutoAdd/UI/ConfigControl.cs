using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

using PicoTorrent.Plugins.AutoAdd.Models;
using PicoTorrent.Plugins.AutoAdd.UI.Dialogs;

namespace PicoTorrent.Plugins.AutoAdd.UI
{
    public partial class ConfigControl : UserControl, IConfigControl
    {
        public ConfigControl()
        {
            InitializeComponent();

            // Events
            _addFolder.Click += addFolder_Click;
            _editFolder.Click += editFolder_Click;
            _foldersContextMenu.Opened += foldersContextMenu_Opened;
            _foldersList.DoubleClick += foldersList_DoubleClick;
            _removeFolder.Click += removeFolder_Click;
        }

        public event EventHandler FoldersChanged;
        
        public IEnumerable<Folder> GetFolders()
        {
            return this.SafeInvoke(() =>
            {
                return _foldersList.Items
                    .Cast<ListViewItem>()
                    .Select(i => (Folder)i.Tag)
                    .ToList();
            });
        }

        public void SetFolders(IEnumerable<Folder> folders)
        {
            this.SafeInvoke(() =>
            {
                _foldersList.Items.Clear();

                foreach (var folder in folders)
                {
                    AddFolder(folder);
                }
            });
        }

        private void AddFolder(Folder folder)
        {
            this.SafeInvoke(() =>
            {
                var lvi = new ListViewItem(folder.Path);
                lvi.SubItems.Add(new ListViewItem.ListViewSubItem(lvi, folder.Pattern));
                lvi.Tag = folder;

                _foldersList.Items.Add(lvi);
            });
        }

        private void EditFolder(ListViewItem item, Folder folder)
        {
            this.SafeInvoke(() =>
            {
                var dlg = new FolderDialog(folder);
                dlg.Text = "Edit folder";

                if (dlg.ShowDialog(this) != DialogResult.OK)
                {
                    return;
                }

                folder = dlg.GetFolder();
                item.SubItems[0].Text = folder.Path;
                item.SubItems[1].Text = folder.Pattern;
                item.Tag = folder;

                FoldersChanged(this, EventArgs.Empty);
            });
        }

        private void addFolder_Click(object sender, EventArgs e)
        {
            this.SafeInvoke(() =>
            {
                var dlg = new FolderDialog();
                dlg.Text = "Add folder";

                if (dlg.ShowDialog(this) == DialogResult.OK)
                {
                    AddFolder(dlg.GetFolder());
                    FoldersChanged(this, EventArgs.Empty);
                }
            });
        }

        private void editFolder_Click(object sender, EventArgs e)
        {
            this.SafeInvoke(() =>
            {
                var selectedItems = _foldersList.SelectedItems;

                if (selectedItems?.Count > 0)
                {
                    var item = selectedItems[0];
                    var folder = item.Tag as Folder;

                    if (folder == null)
                    {
                        return;
                    }

                    EditFolder(item, folder);
                }
            });
        }

        private void foldersContextMenu_Opened(object sender, EventArgs e)
        {
            this.SafeInvoke(() =>
            {
                if (_foldersList.SelectedItems?.Count > 0)
                {
                    _addFolder.Enabled = false;
                    _editFolder.Enabled = true;
                    _removeFolder.Enabled = true;
                }
                else
                {
                    _addFolder.Enabled = true;
                    _editFolder.Enabled = false;
                    _removeFolder.Enabled = false;
                }
            });
        }

        private void foldersList_DoubleClick(object sender, EventArgs e)
        {
            this.SafeInvoke(() =>
            {
                var selectedItems = _foldersList.SelectedItems;

                if (selectedItems?.Count > 0)
                {
                    var item = selectedItems[0];
                    var folder = item.Tag as Folder;

                    if (folder == null)
                    {
                        return;
                    }

                    EditFolder(item, folder);
                }
            });
        }

        private void removeFolder_Click(object sender, EventArgs e)
        {
            this.SafeInvoke(() =>
            {
                var selectedItems = _foldersList.SelectedItems;

                if (selectedItems?.Count > 0)
                {
                    var item = selectedItems[0];
                    _foldersList.Items.Remove(item);

                    FoldersChanged(this, EventArgs.Empty);
                }
            });
        }
    }
}
