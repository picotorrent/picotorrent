using System.Collections.Generic;

using PicoTorrent.Plugins.AutoAdd.Models;

namespace PicoTorrent.Plugins.AutoAdd
{
    public interface IAutoAddConfig
    {
        IEnumerable<Folder> Folders { get; set; }

        int PollInterval { get; set; }
    }
}
