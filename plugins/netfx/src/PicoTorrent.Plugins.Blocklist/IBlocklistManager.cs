namespace PicoTorrent.Plugins.Blocklist
{
    public interface IBlocklistManager
    {
        void Load(string url);

        void Unload();
    }
}
