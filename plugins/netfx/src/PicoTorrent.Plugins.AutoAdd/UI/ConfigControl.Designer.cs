namespace PicoTorrent.Plugins.AutoAdd.UI
{
    partial class ConfigControl
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this._foldersList = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this._foldersContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this._addFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this._editFolder = new System.Windows.Forms.ToolStripMenuItem();
            this._removeFolder = new System.Windows.Forms.ToolStripMenuItem();
            this._foldersContextMenu.SuspendLayout();
            this.SuspendLayout();
            // 
            // _foldersList
            // 
            this._foldersList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
            this._foldersList.ContextMenuStrip = this._foldersContextMenu;
            this._foldersList.Dock = System.Windows.Forms.DockStyle.Fill;
            this._foldersList.FullRowSelect = true;
            this._foldersList.GridLines = true;
            this._foldersList.Location = new System.Drawing.Point(0, 0);
            this._foldersList.MultiSelect = false;
            this._foldersList.Name = "_foldersList";
            this._foldersList.Size = new System.Drawing.Size(357, 269);
            this._foldersList.TabIndex = 0;
            this._foldersList.UseCompatibleStateImageBehavior = false;
            this._foldersList.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Path";
            this.columnHeader1.Width = 260;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Pattern";
            this.columnHeader2.Width = 90;
            // 
            // _foldersContextMenu
            // 
            this._foldersContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this._addFolder,
            this.toolStripSeparator1,
            this._editFolder,
            this._removeFolder});
            this._foldersContextMenu.Name = "_foldersContextMenu";
            this._foldersContextMenu.Size = new System.Drawing.Size(152, 76);
            // 
            // _addFolder
            // 
            this._addFolder.Name = "_addFolder";
            this._addFolder.Size = new System.Drawing.Size(151, 22);
            this._addFolder.Text = "Add folder";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(148, 6);
            // 
            // _editFolder
            // 
            this._editFolder.Name = "_editFolder";
            this._editFolder.Size = new System.Drawing.Size(151, 22);
            this._editFolder.Text = "Edit folder";
            // 
            // _removeFolder
            // 
            this._removeFolder.Name = "_removeFolder";
            this._removeFolder.Size = new System.Drawing.Size(151, 22);
            this._removeFolder.Text = "Remove folder";
            // 
            // ConfigControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.Controls.Add(this._foldersList);
            this.Name = "ConfigControl";
            this.Size = new System.Drawing.Size(357, 269);
            this._foldersContextMenu.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView _foldersList;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ContextMenuStrip _foldersContextMenu;
        private System.Windows.Forms.ToolStripMenuItem _addFolder;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem _editFolder;
        private System.Windows.Forms.ToolStripMenuItem _removeFolder;
    }
}
