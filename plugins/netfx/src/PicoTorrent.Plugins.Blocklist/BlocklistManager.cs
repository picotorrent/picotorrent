using PicoTorrent.Core;
using System;

namespace PicoTorrent.Plugins.Blocklist
{
    public sealed class BlocklistManager : IBlocklistManager
    {
        private readonly ISession _session;

        public BlocklistManager(ISession session)
        {
            if (session == null) throw new ArgumentNullException(nameof(session));
            _session = session;
        }

        public void Load(string url)
        {
        }

        public void Unload()
        {
            // Set an empty blocklist on the session
        }
    }
}
