using System;
using System.Collections.Generic;

using PicoTorrent.Plugins.AutoAdd.Models;

namespace PicoTorrent.Plugins.AutoAdd.UI
{
    public interface IConfigControl
    {
        event EventHandler FoldersChanged;

        IntPtr Handle { get; }

        IEnumerable<Folder> GetFolders();

        void SetFolders(IEnumerable<Folder> folders);
    }
}
