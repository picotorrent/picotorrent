namespace PicoTorrent.Plugins.AutoAdd.Models
{
    public sealed class Folder
    {
        public string Path { get; set; }

        public string Pattern { get; set; }

        public bool RemoveFile { get; set; }
    }
}
