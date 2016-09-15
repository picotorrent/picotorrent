using System;

namespace PicoTorrent.Plugins.Blocklist
{
    public interface IBlocklistConfig
    {
        DateTime? LastUpdatedTime { get; set; }

        string Url { get; set; }
    }
}
