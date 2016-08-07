namespace PicoTorrent.Plugins.AutoAdd.UI.Dialogs
{
    partial class FolderDialog
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this._path = new System.Windows.Forms.TextBox();
            this._pattern = new System.Windows.Forms.TextBox();
            this._browse = new System.Windows.Forms.Button();
            this._ok = new System.Windows.Forms.Button();
            this._removeFile = new System.Windows.Forms.CheckBox();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 3;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 24.82517F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 75.17483F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 82F));
            this.tableLayoutPanel1.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.label2, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this._path, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this._pattern, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this._browse, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this._ok, 2, 3);
            this.tableLayoutPanel1.Controls.Add(this._removeFile, 1, 2);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 4;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(390, 142);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Path";
            // 
            // label2
            // 
            this.label2.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 46);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(41, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Pattern";
            // 
            // _path
            // 
            this._path.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this._path.Location = new System.Drawing.Point(79, 7);
            this._path.Name = "_path";
            this._path.Size = new System.Drawing.Size(225, 20);
            this._path.TabIndex = 3;
            // 
            // _pattern
            // 
            this._pattern.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.SetColumnSpan(this._pattern, 2);
            this._pattern.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._pattern.Location = new System.Drawing.Point(79, 42);
            this._pattern.Name = "_pattern";
            this._pattern.Size = new System.Drawing.Size(308, 21);
            this._pattern.TabIndex = 4;
            this._pattern.Text = "\\.torrent$";
            // 
            // _browse
            // 
            this._browse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this._browse.Location = new System.Drawing.Point(310, 6);
            this._browse.Name = "_browse";
            this._browse.Size = new System.Drawing.Size(77, 23);
            this._browse.TabIndex = 5;
            this._browse.Text = "Browse...";
            this._browse.UseVisualStyleBackColor = true;
            // 
            // _ok
            // 
            this._ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this._ok.Location = new System.Drawing.Point(310, 112);
            this._ok.Name = "_ok";
            this._ok.Size = new System.Drawing.Size(77, 23);
            this._ok.TabIndex = 2;
            this._ok.Text = "Ok";
            this._ok.UseVisualStyleBackColor = true;
            // 
            // _removeFile
            // 
            this._removeFile.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this._removeFile.AutoSize = true;
            this._removeFile.Checked = true;
            this._removeFile.CheckState = System.Windows.Forms.CheckState.Checked;
            this._removeFile.Location = new System.Drawing.Point(79, 79);
            this._removeFile.Name = "_removeFile";
            this._removeFile.Size = new System.Drawing.Size(82, 17);
            this._removeFile.TabIndex = 7;
            this._removeFile.Text = "Remove file";
            this._removeFile.UseVisualStyleBackColor = true;
            // 
            // FolderDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(390, 142);
            this.Controls.Add(this.tableLayoutPanel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FolderDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add folder";
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button _ok;
        private System.Windows.Forms.TextBox _path;
        private System.Windows.Forms.TextBox _pattern;
        private System.Windows.Forms.Button _browse;
        private System.Windows.Forms.CheckBox _removeFile;
    }
}